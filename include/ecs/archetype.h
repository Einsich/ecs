#pragma once
#include <ecs/ecs_std.h>
#include <ecs/hash.h>
#include <ecs/entity_prefab.h>

namespace ecs
{
  enum class SizePolicy
  {
    Singleton = 0u,
    Dozens = 3u,
    Hundreds = 6u,
    Thousands = 9u
  };

  void sort_descriptions_by_names(ecs::vector<ComponentDescription> &descriptions)
  {
    ecs::sort(descriptions.begin(), descriptions.end(), [](const auto &a, const auto &b)
              { return a.name < b.name; });
  }

  struct ComponentContainer
  {
    ComponentDescription description;
    ecs::vector<byte *> data;
    ~ComponentContainer()
    {
      for (byte *ptr : data)
        free(ptr);
    }
    void allocate(uint chunk_size)
    {
      data.push_back((byte *)malloc(chunk_size));
    }
    byte *get(uint chunk, uint offset)
    {
      return data[chunk] + offset;
    }
  };

  struct Archetype
  {
    const uint chunkPower;
    const uint chunkMask;
    const uint chunkSize;
    // uint chunkCount, countInLastChunk;
    uint entityCount; //==chunkSize * chunkCount + countInLastChunk
    ecs::vector<ComponentContainer> components;
    Archetype(ecs::vector<ComponentDescription> &&descriptions, SizePolicy chunk_power)
        : chunkPower((uint)chunk_power), chunkMask((1u << (uint)chunk_power) - 1u), chunkSize(1u << (uint)chunk_power)
    {
      components.resize(descriptions.size());
      sort_descriptions_by_names(descriptions);
      int i = 0;
      for (ComponentContainer &container : components)
      {
        container.description = std::move(descriptions[i]);
        container.allocate(chunkSize);
        i++;
      }
    }

    void add_entity(const EntityPrefab &prefabs_list, EntityPrefab &&overrides_list)
    { 
      const auto &types = get_all_registered_types();
      entityCount++;
      uint chunk = entityCount >> chunkPower;
      uint offset = entityCount & chunkMask;
      uint j = 0;
      const auto &prefabs = prefabs_list.components;
      auto &&overrides = overrides_list.components;
      for (uint i = 0, n = prefabs.size(), m = overrides.size(); i < n; ++i)
      {
        const ComponentPrefab &component = prefabs[i];
        byte *memory = components[i].get(chunk, offset);
        ECS_ASSERT(component.description.nameHash == components[i].description.nameHash);
        if (j < m && overrides[j].description.nameHash == component.description.nameHash)
        {
          types[component.description.typeIndex].moveConstructor(memory, overrides[j].raw_pointer);
          j++;
        }
        else
        {
          types[component.description.typeIndex].copyConstructor(memory, component.raw_pointer);
        }
      }
    }
  };

}