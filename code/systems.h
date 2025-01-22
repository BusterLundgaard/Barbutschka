#ifndef SYSTEMS_H
#define SYSTEMS_H

#include "system.h"

extern Entity_individual_system sys_draw_sprite;
extern Entity_individual_system sys_velocity;

extern Entity_individual_system sys_update_collider_global_pos;

extern Component_individual_system sys_set_prev_pos;

extern Component_individual_system sys_draw_world;

extern Entity_individual_system sys_draw_animation;

extern Component_for_all_system sys_collision;

extern Component_individual_system sys_DEBUG_draw_hit_collider;

extern Entity_individual_system sys_player_movement;
extern Entity_individual_system sys_oscillator_movement;

#endif