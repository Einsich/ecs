#pragma once
#include <ecs/query_description.h>
#include <ecs/archetype_manager.h>

namespace ecs
{

  template <typename OutType, typename InType>
  OutType __forceinline get_smart_component2(InType arg, size_t i)
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
  template <typename... InArgs, typename Callable, typename... OutArgs>
  void __forceinline perform_loop2(Callable function, size_t n, OutArgs... containers)
  {
    for (size_t i = 0; i < n; ++i)
    {
      function(get_smart_component2<InArgs>(containers, i)...);
    }
  }
  template <std::size_t N, typename... Args, typename Callable, std::size_t... Is>
  void __forceinline perform_loop(const ecs::array<const ComponentContainer *, N> &containers, Callable function, size_t i, size_t n, std::index_sequence<Is...>)
  {
    perform_loop2<Args...>(function, n, (std::remove_pointer_t<std::remove_reference_t<Args>> *)(containers[Is] ? containers[Is]->data[i] : nullptr)...);
  }

  template <typename... InArgs, typename Callable, typename... OutArgs>
  void __forceinline perform_query2(Callable function, size_t j, OutArgs... containers)
  {
    function(get_smart_component2<InArgs>(containers, j)...);
  } /*
   template <std::size_t N, typename... Args, typename Callable, std::size_t... Is>
   void __forceinline perform_query(const ecs::vector<ecs::ComponentContainer *> &containers, Callable function, size_t i, size_t j, std::index_sequence<Is...>)
   {
     perform_query2<Args...>(function, j, (std::remove_pointer_t<std::remove_reference_t<Args>> *)(containers[Is] ? containers[Is]->data[i] : nullptr)...);
   } */

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

    for (uint archetypeIdx = 0, archetypeN = cache.archetypes.size(); archetypeIdx < archetypeN; ++archetypeIdx)
    {
      const auto &cachedArchetype = cache.archetypes[archetypeIdx];
      const Archetype &archetype = manager.archetypes[cachedArchetype.archetypeIndex];
      if (archetype.entityCount == 0)
        continue;

      ecs::array<const ComponentContainer *, N> components = get_components<N>(archetype.components, cachedArchetype.componentIndexes, indexes);

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
  void __forceinline perform_query(const QueryCache &cache, EntityId eid, Callable function)
  {
    constexpr uint N = sizeof...(Args);
    constexpr auto indexes = std::make_index_sequence<N>(); /*
     if (eid)
     {
       uint index = eid.array_index();
       int archetypeInd = eid.archetype_index();
       auto it = std::find_if(cache.archetypes.begin(), cache.archetypes.end(),
                              [archetypeInd](const SystemCashedArchetype &cashed_archetype)
                              { return cashed_archetype.archetype->index == archetypeInd; });
       if (it != cache.archetypes.end())
       {
         uint binIdx = index >> binPow;
         uint inBinIdx = index & binMask;
         perform_query<N, Args...>(it->containers, function, binIdx, inBinIdx, indexes);
       }
     } */
  }
#define UNUSED(x) (void)(x)
  template <typename E, typename Event, typename... Args>
  void perform_event(EntityId eid, const E &event, const QueryCache &cache, void (*function)(Event, Args...))
  { /*
     perform_query<Args...>(cache, eid, [&](Args... args)
                            { function(event, args...); }); */
    UNUSED(eid);
    UNUSED(event);
    UNUSED(cache);
    UNUSED(function);
  }
  template <typename R, typename Request, typename... Args>
  void __forceinline perform_request(EntityId eid, R &request, const QueryCache &cache, void (*function)(Request, Args...))
  { /*
     perform_query<Args...>(cache, [&](Args... args)
                            { function(request, args...); }); */
    UNUSED(eid);
    UNUSED(request);
    UNUSED(cache);
    UNUSED(function);
  }
} // namespace ecs
