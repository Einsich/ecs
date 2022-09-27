#pragma once
#include <ecs/type_annotation.h>
#include <ecs/query_description.h>
#include <ecs/chunk_policy.h>
#include <ecs/entity_prefab.h>

namespace ecs
{
  void perform_systems();
  uint add_archetype(ecs::vector<ComponentDescription> &&descriptions, SizePolicy chunk_power);
  uint add_archetype(const ecs::vector<ComponentPrefab> &descriptions, SizePolicy chunk_power);
  void create_entity_immediate(const EntityPrefab &prefabs_list, EntityPrefab &&overrides_list = {}, SizePolicy chunk_power = SizePolicy::Hundreds);
}