#include <ecs/ecs.h>
#include <ecs/type_registration.h>

ECS_TYPE_REGISTRATION(float, "float", true, true, true, {}, {})
ECS_TYPE_REGISTRATION(int, "int", true, true, true, {}, {})
ECS_TYPE_REGISTRATION(bool, "bool", true, true, true, {}, {})