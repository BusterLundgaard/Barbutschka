#include "system.h"

bool operator<(const System_instance& l, const System_instance& r){
    if (l.sys != r.sys) {
        return l.sys < r.sys; // Compare pointers for the `sys` member
    }
    return l.entity_id < r.entity_id; 
}