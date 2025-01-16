#include <system.h>
#include <systems.h>
#include <components.h>

Entity_individual_system sys_player_movement{
    "player_movement",
    {__Player, __Transform, __Velocity, __Animation_player}, 
    [](Ecs_m& em, Id id){
        auto t = em.get_from_entity<_Transform>(id);
        auto v = em.get_from_entity<_Velocity>(id);
        auto p = em.get_from_entity<_Player>(id);
        auto anim = em.get_from_entity<_Animation_player>(id);
        
        _Collider* col = em.get_from_comp<_Collider>(p->ground_trigger_col_id);
        bool grounded = col->hits_solid(em);
        bool p_grounded = col->phits_solid(em);


        if(IsKeyDown(KEY_LEFT)){
            if(grounded){anim->change_animation("walk");}
            v->x = -1.0f;
        }
        else if(IsKeyDown(KEY_RIGHT)){ 
            if(grounded){anim->change_animation("walk");}
            v->x = 1.0f;
        }
        else{
            v->x = 0.0f;
            if(grounded){anim->change_animation("idle");}
        }

        auto moving_platform = col->get_typ_in_hits(__Oscillator, em);
        if(moving_platform){
            auto osc = static_cast<_Oscillator*>(*moving_platform);
            if(!osc->dir){ //Moves in x direction
                v->x += osc->get_speed();
            }
        }

        if(grounded && IsKeyDown(KEY_SPACE)){
            v->y = 2.0f;
            anim->change_animation("jump_start");
        }
        else if(p_grounded && !grounded){
            v->y = std::max(0.0f, v->y);
        }
        else if(!grounded){
            v->y -= 0.04f;
        }

    }
};