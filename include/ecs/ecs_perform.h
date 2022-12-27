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
  OutType PERFORM_INLINE get_component(InType &arg)
  {
    using T = typename std::remove_cvref_t<OutType>;
    if constexpr (is_singleton<T>())
    {
      return *arg;
    }
    else if constexpr (std::is_pointer<T>::value)
    {
      return arg ? arg++ : nullptr;
    }
    else
    {
      return *(arg++);
    }
  }

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
      return arg ? arg + i : nullptr;
    }
    else
    {
      return *(arg + i);
    }
  }
  template <typename... Args, typename... PtrArgs, typename Callable, std::size_t... Is>
  void PERFORM_INLINE perform_loop(std::tuple<PtrArgs...> &&dataPointers, Callable function, size_t n, std::index_sequence<Is...>)
  {
    for (size_t j = 0; j < n; ++j)
    {
      function(get_component<Args>(std::get<Is>(dataPointers))...);
    }
  }

  template <typename... Args, typename... PtrArgs, typename Callable, std::size_t... Is>
  void PERFORM_INLINE perform_query(std::tuple<PtrArgs...> &&dataPointers, Callable function, size_t j, std::index_sequence<Is...>)
  {

    function(get_component<Args>(std::get<Is>(dataPointers), j)...);
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
  typename storage_type<T>::ptr_type PERFORM_INLINE get_component(bool valid_container, const ComponentContainer *container, uint chunk)
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

  template <typename... Args, std::size_t... Is>
  std::tuple<typename storage_type<Args>::ptr_type...> PERFORM_INLINE get_components(const ecs::vector<ComponentContainer> &containers, const ecs::vector<int> &indexes, uint chunk, std::index_sequence<Is...>)
  {
    return {get_component<Args>(*(indexes.data() + Is) >= 0, containers.data() + *(indexes.data() + Is), chunk)...};
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

      uint binN = archetype.entityCount >> archetype.chunkPower;
      for (uint binIdx = 0; binIdx < binN; ++binIdx)
      {
        perform_loop<Args...>(get_components<Args...>(archetype.components, cachedArchetype, binIdx, indexes), function, archetype.chunkSize, indexes);
      }
      uint lastBinSize = archetype.entityCount - (binN << archetype.chunkPower);
      if (lastBinSize > 0)
      {
        perform_loop<Args...>(get_components<Args...>(archetype.components, cachedArchetype, binN, indexes), function, lastBinSize, indexes);
      }
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
        const auto &cachedArchetype = it->second;
        uint binIdx = index >> archetype.chunkPower;
        uint inBinIdx = index & archetype.chunkMask;

        perform_query<Args...>(get_components<Args...>(archetype.components, cachedArchetype, binIdx, indexes), function, inBinIdx, indexes);
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
