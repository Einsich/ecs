#include <ecs/query_manager.h>

namespace ecs
{
  static bool has_potential_data_race(
    const ecs::vector<ecs::ArgumentDescription> &args1,
    const ecs::vector<ecs::ArgumentDescription> &args2)
  {
    for (const auto &arg1 : args1)
    {
      for (const auto &arg2 : args2)
      {
        if (arg1.isSingleton)
        {
          if (arg2.isSingleton && arg1.typeIndex == arg2.typeIndex &&
            (arg2.accessType == ecs::AccessType::ReadWrite ||
            arg1.accessType == ecs::AccessType::ReadWrite))
          {
            return true;
          }
        }
        else if (arg2.fastCompare(arg1) &&
            (arg2.accessType == ecs::AccessType::ReadWrite ||
            arg1.accessType == ecs::AccessType::ReadWrite))
        {
          return true;
        }
      }
    }
    return false;
  }

  void QueryManager::rebuildMultiThreadDescriptions()
  {
    for (auto it1 = allQueryMap.begin(), end = allQueryMap.end(); it1 != end; ++it1)
    {
      auto sys1 = it1->second;
      MultiThreadDescription &descr = sys1->mtDescription;
      if (descr.valid)
        continue;
      //descr.dependsOn.clear();
      descr.hasDataRace.clear();
      //descr.shouldWait.clear();
      auto it2 = it1;
      ++it2;
      for (; it2 != end; ++it2)
      {
        auto sys2 = it2->second;
        MultiThreadDescription &descr2 = sys2->mtDescription;
        if (has_potential_data_race(sys1->arguments, sys2->arguments))
        {
          descr.hasDataRace.push_back(sys2);
          descr2.hasDataRace.push_back(sys1);

          ECS_LOG("systems %s %s have datarace", sys1->name.c_str(), sys2->name.c_str());

        }
      }
      descr.valid = true;
    }
  }
}