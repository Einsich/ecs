#include <ecs/type_registration.h>
#include <ecs/entity_id.h>
#include <ecs/base_events.h>
#include <ecs/query_manager.h>
#include <ecs/event_registration.h>
#include <ecs/ecs.h>
#include <stdarg.h>

namespace ecs
{

  void init(bool register_base_types)
  {
    if (register_base_types)
    {
      type_registration<ecs::EntityId, ecs::PODType>("EntityId");
      type_registration<bool, ecs::PODType>("bool");
      type_registration<int, ecs::PODType>("int");
      type_registration<unsigned int, ecs::PODType>("uint");
      type_registration<float, ecs::PODType>("float");
      type_registration<double, ecs::PODType>("double");
      type_registration<char, ecs::PODType>("char");
      type_registration<unsigned char, ecs::PODType>("byte");
    }
    register_event<ecs::OnEntityCreated>("OnEntityCreated", true);
    register_event<ecs::OnEntityDestroyed>("OnEntityDestroyed", true);
    register_event<ecs::OnEntityTerminated>("OnEntityTerminated", true);
    register_event<ecs::OnSceneCreated>("OnSceneCreated", true);
    register_event<ecs::OnSceneTerminated>("OnSceneTerminated", true);
  }

  void pull_registered_files()
  {
    extern void register_all_pulled_files();
    register_all_pulled_files();

    extern void perform_deffered_events();
    perform_deffered_events();
  }
  
  
  void default_log_function(const char *format, ...)
  {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    printf("\n");
    va_end(args);
  }
  LogFunction ecs_log = &default_log_function;
  LogFunction ecs_error = &default_log_function;

  
  void default_push_function(const char *){}
  void default_pop_function(){}
  
  ProfilerPush ecs_profiler_push = &default_push_function;
  ProfilerPop ecs_profiler_pop = &default_pop_function;
  bool ecs_profiler_enabled = false;
}