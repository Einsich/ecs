#pragma once
#include <ecs/query_description.h>
#include <ecs/archetype_manager.h>

namespace ecs
{

  template <typename OutType, typename InType>
  OutType __forceinline get_component(InType arg, size_t i)
  {
    using cvrefT = typename std::remove_cvref_t<OutType>;
    if constexpr (std::is_pointer<cvrefT>::value)
    {
      return arg ? arg + i : nullptr;
    }
    else
    {
      return *(arg + i);
    }
  }

  template <std::size_t N, typename... Args, typename Callable, std::size_t... Is>
  void __forceinline perform_loop(const ecs::array<const ComponentContainer *, N> &containers, Callable function, size_t i, size_t n, std::index_sequence<Is...>)
  {
    for (size_t j = 0; j < n; ++j)
    {
      function(get_component<Args>((std::remove_pointer_t<std::remove_reference_t<Args>> *)(containers[Is] ? containers[Is]->data[i] : nullptr), j)...);
    }
  }

  template <std::size_t N, typename... Args, typename Callable, std::size_t... Is>
  void __forceinline perform_query(const ecs::array<const ComponentContainer *, N> &containers, Callable function, size_t i, size_t j, std::index_sequence<Is...>)
  {
    function(get_component<Args>((std::remove_pointer_t<std::remove_reference_t<Args>> *)(containers[Is] ? containers[Is]->data[i] : nullptr), j)...);
  }

  template <std::size_t N, std::size_t... Is>
  ecs::array<const ComponentContainer *, N> get_components(const ecs::vector<ComponentContainer> &containers, const ecs::vector<int> &indexes, std::index_sequence<Is...>)
  {
    return {(indexes[Is] >= 0 ? &containers[indexes[Is]] : nullptr)...};
  }

  template <typename... Args, typename Callable>
  void __forceinline perform_query(const QueryCache &cache, Callable function)
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

    for (const auto &p : cache.archetypes)
    {
      const auto &cachedArchetype = p.second;
      const Archetype &archetype = manager.archetypes[p.first];
      if (archetype.entityCount == 0)
        continue;

      ecs::array<const ComponentContainer *, N> components = get_components<N>(archetype.components, cachedArchetype, indexes);

      uint binN = archetype.entityCount >> archetype.chunkPower;
      for (uint binIdx = 0; binIdx < binN; ++binIdx)
      {
        perform_loop<N, Args...>(components, function, binIdx, archetype.chunkSize, indexes);
      }
      uint lastBinSize = archetype.entityCount - (binN << archetype.chunkPower);
      if (lastBinSize > 0)
      {
        perform_loop<N, Args...>(components, function, binN, lastBinSize, indexes);
      }
    }
  }

  template <typename... Args>
  void __forceinline perform_system(const QueryCache &cache, void (*function)(Args...))
  {
    perform_query<Args...>(cache, function);
  }

  template <typename E, typename Event, typename... Args>
  void __forceinline perform_event(const E &event, const QueryCache &cache, void (*function)(Event, Args...))
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
  void __forceinline perform_query(const QueryCache &cache, EntityId eid, Callable function, bool acceptDestoyQueue = false)
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

        ecs::array<const ComponentContainer *, N> components = get_components<N>(archetype.components, cachedArchetype, indexes);

        perform_query<N, Args...>(components, function, binIdx, inBinIdx, indexes);
      }
    }
  }

  template <typename E, typename... Args>
  void perform_event(EntityId eid, const E &event, const QueryCache &cache, void (*function)(const E &, Args...))
  {
    constexpr bool isDestroyEvent =
        std::is_same_v<E, ecs::OnEntityDestoyed> || std::is_same_v<E, ecs::OnEntityTerminated>;
    perform_query<Args...>(
        cache, eid, [&](Args... args)
        { function(event, args...); },
        isDestroyEvent);
  }

  template <typename R, typename... Args>
  void __forceinline perform_request(EntityId eid, R &request, const QueryCache &cache, void (*function)(R &, Args...))
  {
    perform_query<Args...>(
        cache, eid, [&](Args... args)
        { function(request, args...); },
        false);
  }
} // namespace ecs
