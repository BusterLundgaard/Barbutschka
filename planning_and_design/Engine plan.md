# components and systems
for components, you'll have every component in a big enum like

enum component {
    Transform
    Velocity
    Collision
    /// ...
}

every component has some metadata:

struct component_metadata {
    system : function pointer
    coupled_to : list[component] //every other component it reads through pointers, be that component on it's own entity or on another entity
    events : list[component] //a list of events it subscribes to (events each have a struct/component that defines their name and the data they send)
    knows_entity_id : bool //does this component know/need the id of it's entity?   
    size : [here you'll just write size_of(struct_name)]
}

We will store and define all this metadata in a big map from components to component_metadata

Systems are just functions you point to with a specific signature. It's that simple!

You will need to create a list of lists that puts an order on evaluation for the components/their respective systems:

[[Comp1, Comp4, Comp3], [Comp2, Comp8, Comp10], [Comp5], [Comp9, Comp11]]

This would tell the system that [Comp1, Comp4, Comp3] can be executed in any order, but should be executed before any of [Comp2, Comp8, Comp10]. We could implement multithreading if we wanted, but in a small game like ours it'll probably just make things slower. 

# events
Regarding events, we need two structs:

struct event_subscription {
    who : id //the id of the entity subscribing
    event : component //name (enum) of the event you are subscribing to
}

struct event_subscribers {
    subscribers : map from event to vector<id>
}

If some system ´Sys´ of some component ´Comp´ can send events, it must have a pointer in ´Comp´ to an event_subscribers component on it's entity. We may potentially need to make event_subscribers (and hopefully only event_subscribers) a "special" system that has access to not only check if an event_subscription is on an entity, but has access to read it as well. 

# how to store the grid of components and entities
To cleanly store the list of components without having to manually add a new list for every new component you add, we have to make our own custom allocator that can make (and resize) an array using no specific type, just only a specific size. To get good performance, we double the size of the array every time it has reached it's capacity, and similarly cut off half of it's size when it starts using less than half of it's elements.

Doing this will be sort of difficult, and you'll need to look in "malloc business chatgpt.txt" to play around with some examples of how to do these kind of allocations with malloc, realloc and those kinds of things. You'll also need to remmember to free these things!

Now, we just make one of these custom_lists for every component currently in the scene. 

So far, we've only stored the components, with no kind of ordering that defines how they relate to the entities. This will be defined in a seperate table storing just pointers to components. Every row of this matrix will be one type of component, and every column will be an entity. If a certain entity does not store any component of a certain type, it will still store "nil" in the array. 
To store this sparse grid in practice, we make a fixed-size array of vectors (each vector holds pointers to a certain component for all entities currently in the scene)

But wait, how do you know which index of a vector corresponds to which entity id? For this, you'll need a second map from id's to indexes. 


