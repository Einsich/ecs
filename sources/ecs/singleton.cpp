#include <ecs/singleton.h>
#include <ecs/ska/flat_hash_map.hpp>

namespace ecs
{

  struct SingletonStorage
  {
    ecs::vector<byte> data;
    bool inited = false;
  };

  static ska::flat_hash_map<intptr_t, SingletonStorage> singletonStorage;
  bool hasNewSingletonTypes = false;


  void init_singletones()
  {
    auto head = SingletonTypeFabric::linked_list_head;
    while (head)
    {
      auto &storage = singletonStorage[(intptr_t)head];
      if (!storage.inited)
      {
        storage.data.resize(head->sizeOf);
        head->contructor(storage.data.data());
        storage.inited = true;
      }
      head = head->nextFabric;
    }

  }

  void destroy_sinletons()
  {
    auto head = SingletonTypeFabric::linked_list_head;
    while (head)
    {
      auto &storage = singletonStorage[(intptr_t)head];
      if (storage.inited)
      {
        head->destructor(storage.data.data());
        storage.inited = true;
      }
      head = head->nextFabric;
    }
  }

  void *get_singleton(intptr_t idx)
  {
    return singletonStorage[idx].data.data();
  }

}