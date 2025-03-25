#include <system.h>
#include <systems.h>
#include <components.h>
#include <unordered_map>
#include "defs.h"
#include "events.h"
#include "flags.h"
#include "input.h"

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


static const float walk_speed = 40;

static const float T_jump_prep = 0.5;
static float jump_prep_t = 0.0f;
static float jump_forward_t = 0.0f;

static const float b = 2.5f; // Controls how "flat" the arc is, the arc follows y = t^b, so higher b means more flat, more "curved"
static const float c = 1.4f; // the time stretch factor in y=(t/c)^b
static const float small_gravity = 1.5f; //controls the factor of the reduced gravity between max_jump_v0 speed and min_jump_v0 speed
static const float max_jump_v0 = 105;
static const float min_jump_v0 = 80;
static const float max_fall_speed = 400;

// static const float T_arc = 1.2;
// static const float H_min = 10.5;
// static const float H_max = 33.5;
// static const float a = 1.5;
// static const float max_fall_slope = 5.5;

// static const float _t_max_slope = std::pow(max_fall_slope / a, 1/(a-1));
// static const float _C = std::pow(_t_max_slope, a) + (T_arc/2 > _t_max_slope)*max_fall_slope*(T_arc/2 - _t_max_slope); //Just a nasty constant we need


// static float jump_t = 0.0f;
// static float fall_t = 0.0f;
// static float H = 0.0f;

Entity_individual_system sys_player_movement{
    "player_movement",
    {__Player, __Transform, __Velocity, __Animation_player}, {},
    GRAPHIC_LAYER::none,
    __first_frame {
        auto p = em.get_from_entity<_Player>(id);
        auto v = em.get_from_entity<_Velocity>(id);
        p->grounded = false;
        p->state = PLAYER_STATE::FALL;
    },
    __system_events {
        {EVENT::PLAYER_GROUNDED, [](Id id, Event_data data){
            auto p = em.get_from_entity<_Player>(id);
            auto v = em.get_from_entity<_Velocity>(id);
            auto anim = em.get_from_entity<_Animation_player>(id);
            auto grounded_data = std::get<PLAYER_GROUNDED_data>(data);
            
            if(abs(v->y) > 100){
                p->state = PLAYER_STATE::LAND;
                anim->current_anim = "land";
                em.timeout_time(0.5, [id](){  
                    auto p = em.get_from_entity<_Player>(id);
                    auto anim = em.get_from_entity<_Animation_player>(id);
                    p->state = PLAYER_STATE::IDLE;
                    anim->current_anim = "idle";
                });
            } else {
                p->state = PLAYER_STATE::IDLE;
                anim->current_anim = "idle";
            }

            if(grounded_data.oscillator){
                p->on_oscillator = true;
                em.add(_ParentLink(em.get_entity_id(*grounded_data.oscillator)), id);
                v->y = 0.0f;
                v->x = 0.0f;
            } else if(grounded_data.slope){
                //std::cout << "entered slope!" << std::endl;
                auto slope = em.get_from_comp<_Collider>(*grounded_data.slope);
                float slope_angle = atan2(slope->h, slope->slope_dir*slope->w);
                v->x = -cos(*(p->slope_angle))*abs(v->y)*0.4;
                v->y = -sin(*(p->slope_angle))*abs(v->y)*0.4;
            } else {
                v->y = 0.0f;
                v->x = 0.0f;
            }
            p->grounded = true;
        }},
        {EVENT::PLAYER_UNGROUNDED, [](Id id, Event_data data){
            auto p = em.get_from_entity<_Player>(id);
            auto anim = em.get_from_entity<_Animation_player>(id);
            p->slope_angle = std::nullopt;
            if(p->on_oscillator){
                p->on_oscillator = false;
                em.remove_typ(id, __ParentLink);
            }
            p->grounded = false;
            if(p->state != PLAYER_STATE::JUMP){
                p->state = PLAYER_STATE::FALL;
                anim->current_anim="fall";
            }
        }},
        {EVENT::PLAYER_LEFT_SLOPE, [](Id id, Event_data data){
            auto p = em.get_from_entity<_Player>(id);
            p->slope_angle = std::nullopt;
            std::cout << "left slope yay!\n";
        }},
        {EVENT::PLAYER_ENTERED_SLOPE, [](Id id, Event_data data){
            auto p = em.get_from_entity<_Player>(id);
            auto slope = em.get_from_comp<_Collider>(std::get<PLAYER_ENTERED_SLOPE_data>(data).slope);
            p->slope_angle = std::optional<float>(atan2(slope->h, slope->slope_dir*slope->w));
            std::cout << "entered slope the other way!\n";
        }}
    },
    __update {
        auto t = em.get_from_entity<_Transform>(id);
        auto v = em.get_from_entity<_Velocity>(id);
        auto p = em.get_from_entity<_Player>(id);
        auto anim = em.get_from_entity<_Animation_player>(id);
        auto col = em.get_from_entity<_Collider>(id);

        switch(p->state) {
            case PLAYER_STATE::IDLE:
                v->x = 0.95*v->x;
                v->y = 0.95*v->y;
                if(abs(v->x) < 0.1){v->x = 0;}
                if(abs(v->y) < 0.1){v->y = 0;}

                if(Key_Press(KEY_Z)){
                    p->state = PLAYER_STATE::JUMP_PREP;
                    jump_prep_t = 0.0f;
                    jump_forward_t = 0.0f;
                    anim->current_anim="jump_prep";
                }
                if(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)){
                    p->state = PLAYER_STATE::WALK;
                    anim->current_anim = "walk";
                    if(IsKeyDown(KEY_LEFT)){anim->flipped=1;}
                    else{anim->flipped=0;}
                }

                if(IsKeyDown(KEY_DOWN)){
                    p->state = PLAYER_STATE::CROUCH;
                    anim->change_animation("crouch");
                }

                break;
            case PLAYER_STATE::CROUCH:
                v->x = 0.95*v->x;
                v->y = 0.95*v->y;
                if(abs(v->x) < 0.1){v->x = 0;}
                if(abs(v->y) < 0.1){v->y = 0;}

                if(Key_Up(KEY_DOWN)){
                    p->state = PLAYER_STATE::IDLE;
                    anim->change_animation("idle");
                }
                break;

            case PLAYER_STATE::WALK:
                if(IsKeyDown(KEY_LEFT)){
                    anim->flipped=1;
                    if(p->slope_angle){
                        float steepness = std::min(1.0, abs(tan(*(p->slope_angle)))/tan(60*3.14/180));
                        float speed = (*(p->slope_angle) > 3.14/2) ? walk_speed*(1-steepness) : walk_speed*(1+steepness*1.8);
                        v->y = -sin(*(p->slope_angle))*speed;
                        v->x = -speed;
                    } else {
                        v->x = -walk_speed;
                    }
                } 
                else if (IsKeyDown(KEY_RIGHT)){
                    anim->flipped=0;
                    if(p->slope_angle){
                        float steepness = std::min(1.0, abs(tan(*(p->slope_angle)))/tan(60*3.14/180));
                        float speed = (*(p->slope_angle) < 3.14/2) ? walk_speed*(1-steepness) : walk_speed*(1+steepness*1.8);
                        v->y = -sin(*(p->slope_angle))*speed;
                        v->x = speed;
                    } else {
                        v->x = walk_speed;
                    }
                }
                else {
                    p->state = PLAYER_STATE::IDLE;
                    v->x = 0;
                    anim->current_anim = "idle";
                }

                if(Key_Press(KEY_Z)){
                    p->state = PLAYER_STATE::JUMP_PREP;
                    jump_prep_t = 0.0f;
                    jump_forward_t = 0.0f;
                    v->x = 0;
                    anim->current_anim="jump_prep";
                }
                break;

            case PLAYER_STATE::JUMP_PREP:
                jump_prep_t += em.fl;
                if(IsKeyDown(KEY_LEFT)){
                    jump_forward_t -= em.fl;
                } 
                if(IsKeyDown(KEY_RIGHT)){
                    jump_forward_t += em.fl;
                }

                if(Key_Up(KEY_Z)){
                    p->state = PLAYER_STATE::JUMP;
                    v->y = min_jump_v0 + (std::min(jump_prep_t, T_jump_prep) / T_jump_prep)*(max_jump_v0 - min_jump_v0);
                    v->x = v->x + sign(jump_forward_t)*(abs(jump_forward_t)/jump_prep_t)*walk_speed;
                    anim->current_anim="jump_rise";
                }

                break;
            
            case PLAYER_STATE::JUMP:
                if(v->y > min_jump_v0){
                    v->y -= small_gravity;
                } else if (v->y > 0){
                    v->y -= pow(c*abs(v->y)/b, (b-2)/(b-1))*b*(b-1)/(c*c); //this is the second derivative of y=t^b rewritten in terms of it's first derivative, the speed. This way, we don't need to keep track of time.
                } else {
                    p->state = PLAYER_STATE::FALL;
                    std::cout << "Set to fall\n";
                    anim->current_anim="fall";
                }
                break;

            case PLAYER_STATE::FALL:
                if(abs(v->y) < 0.1){
                    v->y = -0.1f;
                }
                if(v->y > -max_fall_speed){
                    v->y -= pow(c*abs(v->y)/b, (b-2)/(b-1))*b*(b-1)/(c*c);
                }
                break;

            case PLAYER_STATE::LAND:
                break;

        }

        p->pgrounded = p->grounded;

    }
};