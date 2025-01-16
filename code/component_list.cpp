#include "toplevel.h"
#include "component.h"
#include "component_list.h"

bool Component_list::check_id_comp(int index, Id comp_id){
    Component* comp = static_cast<Component*>(get(index));
    return static_cast<Component*>(get(index))->comp_id == comp_id;
}
bool Component_list::check_id_entity(int index, Id entity_id){
    Component* comp = static_cast<Component*>(get(index));
    return static_cast<Component*>(get(index))->entity_id == entity_id;
}

bool Component_list::exists_in_queue(Id entity_id){
    for(int i = 0; i < to_be_added_data_queue_size; i++){
        Component* comp = static_cast<Component*>(to_be_added_data + i*comps_metadata.at(typ).bytesize);
        if(comp->entity_id == entity_id){return true;}
    }
    return false;
}


//Public
Component_list::Component_list(Typ typ) : typ(typ), size(0), to_be_added_data_queue_size(0) {
    data = malloc(comps_metadata.at(typ).bytesize*comps_metadata.at(typ).max_elements);

    //This could technically be optimized into something that scales in size and isn't just max_elements, but it'll do for now
    to_be_added_data = malloc(comps_metadata.at(typ).bytesize*comps_metadata.at(typ).max_elements);
}

int Component_list::index_of_comp(Id component_id) {
    //Look forward from search_indexes (most often this is how we'll iterate)
    for(int i = search_index; i<size; i++){
        if(check_id_comp(i, component_id)){
            search_index=i;
            return i;
        }
    }
    //Look backwards if didn't find it in front:
    for(int i = search_index; i >= 0; i--){
        if(check_id_comp(i, component_id)){
            search_index=i;
            return i;
        }
    }
    throw NullPtrException("Could not find component of type " + comps_metadata.at(typ).name + " with component id " + std::to_string(component_id));
}
int Component_list::index_of_entity(Id entity_id) {
    //Look forward from search_indexes (most often this is how we'll iterate)
    for(int i = search_index; i<size; i++){
        if(check_id_entity(i, entity_id)){
            search_index=i;
            return i;
        }
    }
    //Look backwards if didn't find it in front:
    for(int i = search_index; i >= 0; i--){
        if(check_id_entity(i, entity_id)){
            search_index=i;
            return i;
        }
    }
    throw NullPtrException("Could not find component of type " + comps_metadata.at(typ).name + " with entity id " + std::to_string(entity_id));
}
V<int> Component_list::indexes_of_entity(Id entity_id){
    V<int> indexes;
    for(int i = 0; i<size; i++){
        if(check_id_entity(i, entity_id)){
            indexes.push_back(i);
        }
    }
    return indexes;
}

bool Component_list::entity_exists(Id entity_id){
    if(exists_in_queue(entity_id)){return true;}
    for(int i = search_index; i<size; i++){
        if(check_id_entity(i, entity_id)){
            search_index=i;
            return true;
        }
    }
    for(int i = search_index; i >= 0; i--){
        if(check_id_entity(i, entity_id)){
            search_index=i;
            return true;
        }
    }
    return false;
}

Id Component_list::entity_of_comp(Id comp_id){
    return static_cast<Component*>(get(index_of_comp(comp_id)))->entity_id;
}

void Component_list::add(void* el){
    void* dest = to_be_added_data + to_be_added_data_queue_size*comps_metadata.at(typ).bytesize;
    if(comps_metadata.at(typ).has_heap_memory){
        comps_metadata.at(typ).clone(el, dest);
    } else {
        std::memcpy(
            dest, 
            el, 
            comps_metadata.at(typ).bytesize
            );
    }
    to_be_added_data_queue_size++;
}
void Component_list::add_all_in_add_queue() {
    for(int i = 0; i < to_be_added_data_queue_size; i++){
        void* dest = data + size*comps_metadata.at(typ).bytesize;
        void* src = to_be_added_data + i*comps_metadata.at(typ).bytesize;
        if(comps_metadata.at(typ).has_heap_memory){
            comps_metadata.at(typ).clone(src, dest);
            comps_metadata.at(typ).deallocator(src);
        } else {
            std::memcpy(
                dest,
                src, 
                comps_metadata.at(typ).bytesize
            );
        }
        size++;
    }
    to_be_added_data_queue_size=0;
}
void Component_list::remove(int index){
    void* el_src = data + index*comps_metadata.at(typ).bytesize;
    void* last_el_src = data + (size-1)*comps_metadata.at(typ).bytesize;

    if(comps_metadata.at(typ).has_heap_memory){
        comps_metadata.at(typ).deallocator(el_src);
        comps_metadata.at(typ).clone(last_el_src, el_src);
    } else {
        std::memcpy(
            el_src,
            last_el_src,
            comps_metadata.at(typ).bytesize
        );
    }
    
    size--;
}

void* Component_list::get(int index){
    int bitsize = comps_metadata.at(typ).bytesize;
    return data + index*comps_metadata.at(typ).bytesize;
}
void* Component_list::get_from_comp(Id comp_id){
    return get(index_of_comp(comp_id));
}
void* Component_list::get_from_entity(Id entity_id){
    return get(index_of_entity(entity_id));
}

