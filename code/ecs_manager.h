#include "components.h"

class ECS_manager {
    std::vector<System*> systems;
    std::unordered_map<std::type_index, BaseComponentMap*> component_maps;
    
    void remove_component(std::type_index typ, int16_t comp_id);
    void remove_components(std::type_index typ, int16_t entity_id);
    
    public:
    ECS_manager(std::unordered_map<std::type_index, BaseComponentMap*> typ_map, std::vector<System*> systems);
    
    void add_component(int16_t entity_id, Component* comp);
    bool has_component(int16_t entity_id, std::type_index typ);

    Component* get_component(int16_t comp_id, std::type_index typ);
    std::vector<Component*> get_components(int16_t entity_id, std::type_index typ);

    int16_t get_entity(int16_t component_id, std::type_index typ);

    Component* get_global_component(std::type_index typ);

    void update();

    std::string get_debug_table();
};