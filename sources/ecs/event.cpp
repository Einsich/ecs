#include <ecs/type_description_storage.h>
#include <ecs/event.h>

USE_DESCRIPTION_STORAGE(ecs::EventTypeDescription, register_event, event_name_to_index, get_all_registered_events, get_next_event_index)