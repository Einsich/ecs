#pragma once
#include <ecs/ecs_std.h>

namespace ecs
{
  enum class EntityState
  {
    Invalid,
    CreatedNotInited, // async creation
    CreatedAndInited,
    InDestroyQueue,
    Destoyed
  };
  struct EntityDescription
  {
    uint archetype = -1;
    uint index = -1;
    EntityState state = EntityState::Invalid;

    bool valid_for_destroy() const
    {
      return state == EntityState::CreatedAndInited || state == EntityState::CreatedNotInited || state == EntityState::InDestroyQueue;
    }
  };
  struct EntityId
  {
  private:
    const EntityDescription *description = nullptr;

  public:
    EntityId() = default;
    EntityId(const EntityDescription *description) : description(description) {}

    bool valid() const
    {
      return description
                 ? description->state == EntityState::CreatedAndInited
                 : false;
    }

    bool get_info(uint &archetype, uint &index, EntityState &state)
    {
      if (description)
      {
        archetype = description->archetype;
        index = description->index;
        state = description->state;
        return true;
      }
      return false;
    }
    friend struct EntityPool;
  };
}