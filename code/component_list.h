#ifndef COMPONENT_LIST_H
#define COMPONENT_LIST_H

#include "defs.h"
#include "component.h"

class Component_list {
    private:
    const Typ typ;    
    void* data;
    void* to_be_added_data;
    int to_be_added_data_queue_size;

    int search_index=0;

    bool check_id_comp(int index, Id comp_id);
    bool check_id_entity(int index, Id entity_id);

    bool exists_in_queue(Id entity_id);

    public:
    int size;

    Component_list(Typ typ);

    int index_of_comp(Id component_id);
    int index_of_entity(Id entity_id);
    V<int> indexes_of_entity(Id entity_id);

    bool entity_exists(Id entity_id);

    Id entity_of_comp(Id comp_id);
    
    void add(void* el);
    void add_all_in_add_queue();
    void remove(int index);

    void* get(int index);
    void* get_from_comp(Id comp_id);
    void* get_from_entity(Id entity_id);
};

#endif