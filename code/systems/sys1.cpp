#include "../components.h"
#include "../ecs_manager.h"
#include "../systems.h"

Entity_System_Individual sys_draw_sprite(
    "draw_sprite_system",
    {typeid(_Sprite), typeid(_Transform)},
    entity_individual_signature {
        param(_Sprite, s)(comps[0][0]);
        param(_Transform, t)(comps[1][0]);
        DrawTexture(s->tex, t->x, t->y, WHITE);
    }
);

Entity_System_Individual sys_velocity(
    "velocity_system",
    {typeid(_Velocity), typeid(_Transform)},
    entity_individual_signature {
        param(_Velocity, v)(comps[0][0]);
        param(_Transform, t)(comps[1][0]);
        t->x += v->x;
        t->y += v->y;
    }
);
