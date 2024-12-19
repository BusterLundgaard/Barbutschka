#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "components.h"

extern Entity_System_Individual sys_draw_sprite;
extern Entity_System_Individual sys_velocity;

extern Entity_System_Individual sys_update_collider_global_pos;
extern All_Component_System sys_find_collider_hits;
extern Entity_System_Individual sys_DEBUG_draw_hit_collider;

extern Entity_System_Individual sys_draw_world;

#endif