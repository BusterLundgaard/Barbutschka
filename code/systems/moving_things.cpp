#include <system.h>
#include <systems.h>
#include <components.h>
#include "events.h"

Entity_individual_system sys_oscillator_movement{
    "oscillator_movement",
    {__Oscillator, __Velocity},
    //First frame
    [](Ecs_m& em, Id id){ 
        auto v = em.get_from_entity<_Velocity>(id);
        auto ol = em.get_from_entity<_Oscillator>(id);
        if(ol->dir){
            v->y += 2*(ol->distance)/ol->period;
        } else {
            v->x += 2*(ol->distance)/ol->period;
        }
        
    },
    //Update
    [](Ecs_m& em, Id id){
        auto v = em.get_from_entity<_Velocity>(id);
        auto ol = em.get_from_entity<_Oscillator>(id);

        ol->tick(em.fl);
        if(ol->time > ol->period){
            em.emit_event(Event::OSCILLATOR_CHANGE, Event_data(OSCILLATOR_CHANGE_data{.direction=ol->dir}), 4);

            if(ol->dir){
                if(ol->forwards){v->y -= 4*(ol->distance)/ol->period;} else {v->y += 4*(ol->distance)/ol->period;}
            } else {
                if(ol->forwards){v->x -= 4*(ol->distance)/ol->period;} else {v->x += 4*(ol->distance)/ol->period;}
            }
            ol->forwards = !ol->forwards;
            ol->reset();
        }
    }
};
