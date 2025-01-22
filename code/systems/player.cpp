#include <system.h>
#include <systems.h>
#include <components.h>
#include <unordered_map>
#include "defs.h"
#include "events.h"

Entity_individual_system sys_player_movement{
    "player_movement",
    {__Player, __Transform, __Velocity, __Animation_player}, 
    __first_frame {},
    __system_events {},
    __update {
        auto t = em.get_from_entity<_Transform>(id);
        auto v = em.get_from_entity<_Velocity>(id);
        auto p = em.get_from_entity<_Player>(id);
        auto anim = em.get_from_entity<_Animation_player>(id);
        
        _Collider* col = em.get_from_comp<_Collider>(p->ground_trigger_col_id);
        bool grounded = col->hits_solid(em);
        bool p_grounded = col->phits_solid(em);

        if(grounded && IsKeyDown(KEY_LEFT)){
            if(grounded){anim->change_animation("walk");}
            v->x = -50.0f;
        }
        else if(grounded && IsKeyDown(KEY_RIGHT)){ 
            if(grounded){anim->change_animation("walk");}
            v->x = 50.0f;
        }
        else if(grounded){
            v->x = 0.0f;
            if(grounded){anim->change_animation("idle");}
        }

        auto moving_platform = col->get_typ_in_hits(__Oscillator, em);
        if(moving_platform){
            auto osc = static_cast<_Oscillator*>(*moving_platform);
            if(!osc->dir){ //Moves in x direction
                float max_speed = abs(osc->get_speed())+50.0f;
                v->x = std::clamp(v->x, -50.0f, 50.0f);
                v->x = std::clamp(v->x + osc->get_speed(), -max_speed, max_speed);
            }
        }

        if(grounded && IsKeyDown(KEY_SPACE)){
            v->y = 125.0f;
            anim->change_animation("jump_start");
        }
        else if(p_grounded && !grounded){
            v->y = std::max(0.0f, v->y);
        }
        else if(!grounded){
            v->y -= 3.0f;
        }

    }
};