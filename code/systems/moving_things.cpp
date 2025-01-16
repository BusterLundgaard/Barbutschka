#include <system.h>
#include <systems.h>
#include <components.h>

Entity_individual_system sys_oscillator_movement{
    "oscillator_movement",
    {__Oscillator, __Velocity},
    //First frame
    [](Ecs_m& em, Id id){ 
        auto v = em.get_from_entity<_Velocity>(id);
        auto ol = em.get_from_entity<_Oscillator>(id);
        if(ol->dir){
            v->y += 1;
        } else {
            v->x += 1;
        }
        
    },
    //Update
    [](Ecs_m& em, Id id){
        auto v = em.get_from_entity<_Velocity>(id);
        auto ol = em.get_from_entity<_Oscillator>(id);

        ol->tick();
        if(ol->time > ol->period){
            if(ol->dir){
                if(ol->forwards){v->y -= 2;} else {v->y += 2;}
            } else {
                if(ol->forwards){v->x -= 2;} else {v->x += 2;}
            }
            ol->forwards = !ol->forwards;
            ol->reset();
        }
    }
};
