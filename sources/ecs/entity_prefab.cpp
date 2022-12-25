#include <ecs/type_annotation.h>
#include <ecs/entity_prefab.h>
#include <ecs/entity_id.h>
#include <ecs/ska/flat_hash_map.hpp>

namespace ecs
{
  EntityPrefab::EntityPrefab(const char *name, ecs::vector<ComponentPrefab> &&components_, SizePolicy chunk_power)
      : name(name), components(std::move(components_)), chunkPower(chunk_power)
  {
    bool have_collisions = false;
    const auto &types = get_all_registered_types();
    for (const auto &comp : components)
    {
      if (comp.typeIndex == -1u)
      {
        have_collisions = true;
        break;
      }
      if (types[comp.typeIndex].awaitConstructor)
      {
        requireAwaitCreation = true;
        break;
      }
    }
    static ComponentPrefab entityIdPrefab = ComponentPrefab("eid", EntityId());
    components.push_back(entityIdPrefab);
    sort_prefabs_by_names(components);

    for (uint i = 1, n = components.size(); i < n; i++)
    {
      if (components[i].nameHash == components[i - 1].nameHash)
      {
        have_collisions = true;
        ECS_ERROR("components with same name \"%s\" in \"%s\" prefab, with types <%s> <%s>",
                  components[i].name.c_str(),
                  components[i-1].name.c_str(),
                  type_name(components[i].typeIndex),
                  type_name(components[i - 1].typeIndex));
      }
    }
    if (have_collisions)
      components.clear();
  }
  using NameTable = ska::flat_hash_map<ecs::string, prefab_id, ska::power_of_two_std_hash<ecs::string>>;

  struct PrefabStorage
  {
    ecs::vector<EntityPrefab> prefabs;
    NameTable prefab_name_to_id;
  };

  static PrefabStorage storage;

  prefab_id create_entity_prefab(EntityPrefab &&prefab)
  {
    if (prefab.components.empty())
    {
      ECS_ERROR("registration of existing prefab %s", prefab.name.c_str());
      return invalidPrefabId;
    }
    auto it = storage.prefab_name_to_id.find(prefab.name);
    if (it == storage.prefab_name_to_id.end())
    {
      prefab_id id = storage.prefabs.size();
      storage.prefab_name_to_id.emplace(prefab.name, id);
      storage.prefabs.emplace_back(std::move(prefab));
      return id;
    }
    ECS_ERROR("registration of existing prefab %s", prefab.name.c_str());
    return invalidPrefabId;
  }

  prefab_id get_prefab_id(const char *name)
  {
    auto it = storage.prefab_name_to_id.find_as(name);
    if (it != storage.prefab_name_to_id.end())
    {
      return it->second;
    }
    ECS_ERROR("prefab %s doesn't exist", name);
    return invalidPrefabId;
  }

  prefab_id get_prefab_id(const ecs::string &name)
  {
    return get_prefab_id(name.c_str());
  }

  const EntityPrefab &get_prefab(prefab_id id)
  {
    return storage.prefabs[id];
  }
}