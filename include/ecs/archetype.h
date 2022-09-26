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

  inline void sort_descriptions_by_names(ecs::vector<ComponentDescription> &descriptions)
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
    uint entityCount = 0; //==chunkSize * chunkCount + countInLastChunk
    uint totalCapacity = 0;
    ecs::vector<ComponentContainer> components;
    Archetype(ecs::vector<ComponentDescription> &&descriptions, SizePolicy chunk_power);

    void update_capacity();

    void ECS_INLINE get_chunk_offset(uint idx, uint &chunk, uint &offset)
    {
      chunk = idx >> chunkPower;
      offset = idx & chunkMask;
    }

    void add_entity(const EntityPrefab &prefabs_list, EntityPrefab &&overrides_list);

    void destroy_entity(uint idx);

    void destroy_all_entities();
  };

}