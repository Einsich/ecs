#pragma once
#include <ecs/ecs_std.h>
#include <ecs/hash.h>
#include <ecs/entity_prefab.h>
#include <ecs/chunk_policy.h>

namespace ecs
{
  struct EntityDescription;
  struct EntityPool;
  inline static bool component_comparator(const ComponentDescription &a, const ComponentDescription &b)
  {
    return a.name < b.name;
  };

  inline void sort_descriptions_by_names(ecs::vector<ComponentDescription> &descriptions)
  {
    ecs::sort(descriptions.begin(), descriptions.end(), component_comparator);
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
    byte *get(uint chunk, uint offset, uint sizeoff) const
    {
      return data[chunk] + offset * sizeoff; 
    }
  };

  struct Archetype
  {
    const uint chunkPower;
    const uint chunkMask;
    const uint chunkSize;
    // uint chunkCount, countInLastChunk;
    uint entityCount = 0; //==chunkSize * chunkCount + countInLastChunk
    uint totalCapacity = 0;
    ecs::vector<ComponentContainer> components;
    ComponentContainer *eidContainer = nullptr; // ptr to components

    Archetype(ecs::vector<ComponentDescription> &&descriptions, SizePolicy chunk_power);

    void update_capacity();

    void ECS_INLINE get_chunk_offset(uint idx, uint &chunk, uint &offset)
    {
      chunk = idx >> chunkPower;
      offset = idx & chunkMask;
    }

    void add_entity(EntityDescription *entity, const EntityPrefab &prefabs_list, ecs::vector<ComponentPrefab> &&overrides_list);

    void destroy_entity(uint idx);

    void destroy_all_entities(EntityPool &entity_pool);

    int findComponentIndex(const ComponentDescription &descr) const;
  };

}