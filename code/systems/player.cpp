#include <system.h>
#include <systems.h>
#include <components.h>
#include <unordered_map>
#include "defs.h"
#include "events.h"
#include "flags.h"

Entity_individual_system debug_collision {
    "debug_collision",
    {__DebugCollision, __Transform, __Velocity}, {F_Debug_collision},
    GRAPHIC_LAYER::none,
    __update {
        auto t = em.get_from_entity<_Transform>(id);
        auto v = em.get_from_entity<_Velocity>(id);

        if(IsKeyDown(KEY_LEFT)){
            v->x = -50;
        } 
        else if(IsKeyDown(KEY_RIGHT)){
            v->x = 50;
        } else {v->x = 0;}

        if(IsKeyDown(KEY_DOWN)){
            v->y = -50;
        } 
        else if(IsKeyDown(KEY_UP)){
            v->y = 50;
        } else {v->y = 0;}

    }
};

Entity_individual_system sys_player_movement{
    "player_movement",
    {__Player, __Transform, __Velocity, __Animation_player}, {},
    GRAPHIC_LAYER::none,
    __first_frame {},
    __system_events {
        {EVENT::PLAYER_GROUNDED, [](Ecs_m& em, Id id, Event_data data){
            auto p = em.get_from_entity<_Player>(id);
            auto grounded_data = std::get<PLAYER_GROUNDED_data>(data);
            if(grounded_data.oscillator){
                std::cout << "Grounded on oscilattor!\n";
                p->on_oscillator = true;
            }
            p->grounded = true;
        }},
        {EVENT::PLAYER_UNGROUNDED, [](Ecs_m& em, Id id, Event_data data){
            auto p = em.get_from_entity<_Player>(id);
            if(p->on_oscillator){
                std::cout << "Ungrounded from oscilattor!\n";
                p->on_oscillator = false;
            }
            p->grounded = false;
        }}
    },
    __update {
        auto t = em.get_from_entity<_Transform>(id);
        auto v = em.get_from_entity<_Velocity>(id);
        auto p = em.get_from_entity<_Player>(id);
        auto anim = em.get_from_entity<_Animation_player>(id);
        auto col = em.get_from_entity<_Collider>(id);

        v->x = 0.0f;
        if(IsKeyDown(KEY_LEFT)){
            v->x = -50.0f;
        }
        else if(IsKeyDown(KEY_RIGHT)){
            v->x = 50.0f;
        }

        if(p->grounded && IsKeyDown(KEY_SPACE)){
            v->y = 100.0f;
        }
        else if(!p->grounded){
            v->y -= 4.0f;
        } else {
            v->y = -1.0f;
        }



        // bool grounded = GRID[int() ][int(t->x + col->x)/BLOCK_SIZE]
        
        // _Collider* col_ground = em.get_from_comp<_Collider>(p->ground_trigger_col_id);
        // bool grounded = col_ground->hits_solid(em);
        // bool p_grounded = col_ground->phits_solid(em);

        // _Collider* col_fall = em.get_from_comp<_Collider>(p->falling_trigger_col_id);
        // bool falling = !(col_fall->hits_solid(em));

        // if(IsKeyDown(KEY_LEFT)){
        //     if(grounded){anim->change_animation("walk");}
        //     v->x = -50.0f;
        // }
        // else if(IsKeyDown(KEY_RIGHT)){ 
        //     if(grounded){anim->change_animation("walk");}
        //     v->x = 50.0f;
        // }
        // else if(grounded){
        //     v->x = 0.0f;
        //     if(grounded){anim->change_animation("idle");}
        // }

        // auto moving_platform = col_ground->get_typ_in_hits(__Oscillator, em);
        // if(moving_platform){
        //     auto osc = static_cast<_Oscillator*>(*moving_platform);
        //     if(!osc->dir){ //Moves in x direction
        //         float max_speed = abs(osc->get_speed())+50.0f;
        //         v->x = std::clamp(v->x, -50.0f, 50.0f);
        //         v->x = std::clamp(v->x + osc->get_speed(), -max_speed, max_speed);
        //     }
        // }

        // if(grounded && IsKeyDown(KEY_SPACE)){
        //     v->y = 145.0f;
        //     anim->change_animation("jump_start");
        // }
        // else if(!p_grounded && grounded){
        //     em.timeout(3, [&em,id](){    
        //         auto v = em.get_from_entity<_Velocity>(id);
        //         if(v->y < 0){v->y = 0.0f;}
        //     });
        // }
        // else if(!p->grounded){
        //     v->y -= 3.0f;
        // }

        p->pgrounded = p->grounded;

    }
};