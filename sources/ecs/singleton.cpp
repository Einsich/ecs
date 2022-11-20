#include <ecs/type_description_storage.h>
#include <ecs/singleton.h>

USE_DESCRIPTION_STORAGE(ecs::SingletonDescription, register_singleton, singleton_name_to_index, get_all_registered_singletons, get_next_singleton_index)

namespace ecs
{

  struct SingletonStorage
  {
    ecs::vector<byte> data;
    bool inited = false;
  };

  static ecs::vector<SingletonStorage> singletonStorage;
  bool hasNewSingletonTypes = false;

  void preallocate_singletones()
  {
    if (hasNewSingletonTypes)
    {
      const auto &types = ecs::get_all_registered_singletons();

      uint i = singletonStorage.size();
      uint n = types.size();
      singletonStorage.resize(types.size());
      for (; i < n; i++)
      {
        singletonStorage[i].data.resize(types[i].sizeOf);
      }
      hasNewSingletonTypes = false;
    }
  }

  void init_singletones()
  {
    ECS_ASSERT(!hasNewSingletonTypes);

    const auto &types = ecs::get_all_registered_singletons();
    singletonStorage.resize(types.size());
    for (uint i = 0, n = types.size(); i < n; i++)
    {
      if (!singletonStorage[i].inited)
      {
        types[i].contructor(singletonStorage[i].data.data());
        singletonStorage[i].inited = true;
      }
    }
  }

  void destroy_sinletons()
  {
    const auto &types = ecs::get_all_registered_singletons();
    for (uint i = 0, n = types.size(); i < n; i++)
    {
      if (singletonStorage[i].inited)
      {
        types[i].destructor(singletonStorage[i].data.data());
        singletonStorage[i].inited = false;
      }
    }
  }

  void *get_singleton(uint idx)
  {
    singletonStorage[idx].data.data();
  }

}