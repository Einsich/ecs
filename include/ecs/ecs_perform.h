#pragma once
#include <ecs/query_description.h>
#include <ecs/archetype_manager.h>

namespace ecs
{
#define PERFORM_INLINE __forceinline

  template <typename T>
  struct clear_type
  {
    using type = std::remove_pointer_t<std::remove_reference_t<T>>;
  };


  template <typename OutType, typename InType>
  OutType PERFORM_INLINE get_component(InType arg, uint i)
  {
    using T = typename std::remove_cvref_t<OutType>;
    if constexpr (is_singleton<T>())
    {
      return *arg;
    }
    else if constexpr (std::is_pointer<T>::value)
    {
      __builtin_prefetch(arg + i + 1, 0);
      return arg ? arg + i : nullptr;
    }
    else
    {
      __builtin_prefetch(arg + i + 1, 0);
      return *(arg + i);
    }
  }


  template <typename T, typename T2 = void>
  struct storage_type
  {
    using ptr_type = typename clear_type<T>::type *__restrict;
    using type = typename clear_type<T>::type;
  };

  // This is a partial specialisation, not a separate template.
  template <typename T>
  struct storage_type<T, typename std::enable_if<ecs::is_singleton<typename clear_type<T>::type>()>::type>
  {
    using ptr_type = typename clear_type<T>::type *__restrict;
    using type = typename clear_type<T>::type;
  };

  template <typename T>
  typename storage_type<T>::ptr_type PERFORM_INLINE get_container(bool valid_container, const ComponentContainer *container, uint chunk)
  {
    if constexpr (is_singleton<typename storage_type<T>::type>())
    {
      return get_singleton<typename storage_type<T>::type>();
    }
    else
    {
      return (typename storage_type<T>::ptr_type)(valid_container ? *(container->data.data() + chunk) : nullptr);
    }
  }

  template <typename... Args, typename Callable>
  void PERFORM_INLINE perform_inner_loop(typename storage_type<Args>::ptr_type ...containers, Callable function, size_t n)
  {
    for (size_t j = 0; j < n; ++j)
    {
      function(get_component<Args>(containers, j)...);
    }
  }

  template <typename... Args, typename Callable, std::size_t... Is>
  void PERFORM_INLINE perform_loop(uint entity_count, uint chunk_power, uint chunk_size, const ComponentContainer *containers, const int *indexes, Callable function, std::index_sequence<Is...>)
  {
    uint binN = entity_count >> chunk_power;
    for (uint binIdx = 0; binIdx < binN; ++binIdx)
    {
      perform_inner_loop<Args...>(get_container<Args>(*(indexes + Is) >= 0, containers + *(indexes + Is), binIdx)..., function, chunk_size);
    }
    uint lastBinSize = entity_count - (binN << chunk_power);
    if (lastBinSize > 0)
    {
      perform_inner_loop<Args...>(get_container<Args>(*(indexes + Is) >= 0, containers + *(indexes + Is), binN)..., function, lastBinSize);
    }
  }

  template <typename... Args, typename Callable, std::size_t... Is>
  void PERFORM_INLINE perform_query(const ComponentContainer *containers, const int *indexes, uint chunk, Callable function, size_t j, std::index_sequence<Is...>)
  {

    function(get_component<Args>(get_container<Args>(*(indexes + Is) >= 0, containers + *(indexes + Is), chunk), j)...);
  }
  template <typename... Args, typename Callable>
  void PERFORM_INLINE perform_query(const QueryCache &cache, Callable function)
  {
    constexpr uint N = sizeof...(Args);
    constexpr auto indexes = std::make_index_sequence<N>();
    const ArchetypeManager &manager = get_archetype_manager();

    if constexpr (N == 0)
    {
      if (cache.noArchetype)
      {
        function();
        return;
      }
    }

    if constexpr (std::conjunction_v<is_singleton_type<typename clear_type<Args>::type>...>)
    {
      function(*get_singleton<typename clear_type<Args>::type>()...);
      return;
    }

    for (const auto &p : cache.archetypes)
    {
      const auto &cachedArchetype = p.second;
      const Archetype &archetype = manager.archetypes[p.first];
      if (archetype.entityCount == 0)
        continue;
      const int *cachedComponentIdx = cachedArchetype.data();
      const ComponentContainer *containers = archetype.components.data();

      perform_loop<Args...>(archetype.entityCount, archetype.chunkPower, archetype.chunkSize, containers, cachedComponentIdx, function, indexes);
    }
  }

  template <typename... Args>
  void PERFORM_INLINE perform_system(const QueryCache &cache, void (*function)(Args...))
  {
    perform_query<Args...>(cache, function);
  }

  template <typename E, typename Event, typename... Args>
  void PERFORM_INLINE perform_event(const E &event, const QueryCache &cache, void (*function)(Event, Args...))
  {
    perform_query<Args...>(cache, [&](Args... args)
                           { function(event, args...); });
  }
  template <typename R, typename Request, typename... Args>
  void __forceinline perform_request(R &request, const QueryCache &cache, void (*function)(Request, Args...))
  {
    perform_query<Args...>(cache, [&](Args... args)
                           { function(request, args...); });
  }

  template <typename... Args, typename Callable>
  void PERFORM_INLINE perform_query(const QueryCache &cache, EntityId eid, Callable function, bool acceptDestoyQueue = false)
  {
    constexpr uint N = sizeof...(Args);
    constexpr auto indexes = std::make_index_sequence<N>();
    uint archetypeIdx, index;
    EntityState state;
    if (eid.get_info(archetypeIdx, index, state) &&
        (state == EntityState::CreatedAndInited || (acceptDestoyQueue && state == EntityState::InDestroyQueue)))
    {
      auto it = cache.archetypes.find(archetypeIdx);
      if (it != cache.archetypes.end())
      {
        const ArchetypeManager &manager = get_archetype_manager();
        const Archetype &archetype = manager.archetypes[archetypeIdx];
        const ecs::vector<int> &cachedArchetype = it->second;
        uint binIdx = index >> archetype.chunkPower;
        uint inBinIdx = index & archetype.chunkMask;

        perform_query<Args...>(archetype.components.data(), cachedArchetype.data(), binIdx, function, inBinIdx, indexes);
      }
    }
  }

  template <typename E, typename... Args>
  void PERFORM_INLINE perform_event(EntityId eid, const E &event, const QueryCache &cache, void (*function)(const E &, Args...))
  {
    constexpr bool isDestroyEvent =
        std::is_same_v<E, ecs::OnEntityDestroyed> || std::is_same_v<E, ecs::OnEntityTerminated>;
    perform_query<Args...>(
        cache, eid, [&](Args... args)
        { function(event, args...); },
        isDestroyEvent);
  }

  template <typename R, typename... Args>
  void PERFORM_INLINE perform_request(EntityId eid, R &request, const QueryCache &cache, void (*function)(R &, Args...))
  {
    perform_query<Args...>(
        cache, eid, [&](Args... args)
        { function(request, args...); },
        false);
  }
} // namespace ecs
