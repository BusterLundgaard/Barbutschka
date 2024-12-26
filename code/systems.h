#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "components.h"

extern Entity_system_individual sys_draw_sprite;
extern Entity_system_individual sys_velocity;
extern Entity_system_individual sys_update_collider_global_pos;

extern Component_system_all sys_find_collider_hits;
extern Component_system_individual sys_DEBUG_draw_hit_collider;
extern Component_system_individual sys_draw_world;
extern Component_system_individual sys_set_prev_pos;

extern Entity_system_individual sys_debug_move;

#endif