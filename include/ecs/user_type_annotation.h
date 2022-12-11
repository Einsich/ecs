#pragma once
#include <ecs/ecs_std.h>
#include <ecs/type_annotation.h>

namespace ecs
{
  template <typename Info>
  ecs::vector<Info> &get_all_user_info()
  {
    static ecs::vector<Info> user_info;
    return user_info;
  }

  template <typename Info>
  void add_user_info(uint type_idx, Info &&info)
  {
    auto &infoList = get_all_user_info<Info>();
    infoList.resize(TypeIndex<T>::value + 1);
    infoList[TypeIndex<T>::value] = info;
  }

  template <typename T, typename Info>
  void add_user_info(Info &&info)
  {
    auto &infoList = get_all_user_info<Info>();
    infoList.resize(type_idx + 1);
    infoList[type_idx] = info;
  }

  template <typename Info>
  Info &get_user_info(int type_idx)
  {
    return get_all_user_info<Info>()[type_idx];
  }

  template <typename T, typename Info>
  Info &get_user_info()
  {
    return get_all_user_info<T>()[TypeIndex<T>::value];
  }

  template <typename T, typename Info>
  struct UserInfoRegistrationHelper
  {
    UserInfoRegistrationHelper(Info &&info)
    {
      add_user_info<T, Info>(std::move(info));
    }
  };

#define ECS_USER_TYPE_REGISTRATION(TYPE, INFO) \
  static ecs::UserInfoRegistrationHelper<TYPE, INFO> \
      __CONCAT__(user_type_registrator, __LINE__)(INFO);
}