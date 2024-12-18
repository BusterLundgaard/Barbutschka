//prefixes and convention:
T_name : Tag. A component used for communication. ALWAYS deletes itself after one frame

struct transform {
    x : int
    y : int
}

struct velocity {
    x : int
    y : int
}

struct kinematic_object {
    e : id
    t : *transform
    v : *velocity
    inertia : *velocity
}

system velocity_system(kinematic_object):
sets transform using speed every frame
does no kind of checking

// For handling moving platforms
struct inertia_link {
    master : *kinematic_object
    slave : *kinematic_object
}

system gravity_system(kinematic_object):
decreases velocity by gravity *if* entity e has "in_air" tag. 

struct rect {
    x : int
    y : int
    w : int
    h : int
}

struct collision_rect {
    e : id
    t : transform
    r : rect
    trigger_only : bool
    layer : enum
}

# collisions 
#WORLD_ID = 0
//everybody can access this, world is always 0

struct world {
    colliders : colliders
    room_id : int
}

struct colliders {
    dynamic : list[collison_rect]
    static : grid of bools
}
struct tag_new_collider {
    c : *collision_rect
}
struct dynamic_collision_hit {
    other : id
    // this will be added to the other part of the collision
}
struct static_collision_adjustment {
    adjustment_x : int
    adjustment_y : int
    transform : transform
    velocity : velocity
}

system collision_system(colliders):
    for every new_collision component: add this collision to dynamic list
    for every collision between two collision_rects on layers that can collide, add a dynamic_collision_hit component to each of them  

    if dynamic is not trigger and is inside static collision, add static_collision_adjustment component to it
    (don't know how exactly we will determine it yet)
    (we might potentially need to know something about the velocity of the colliding object, ie where it was before)

    the collision system should both create tags for *new* collisions (collided now but not last frame) and tags for *removed* collisions 

    using this, the individual player/entity/whatever can themselves figure out if they are grounded

    it is up to the player system to add the inertia link.

system static_collision_adjustment_system(static_collision_adjustment):
    just adjust the transform and velocity

system collision_hit_cleanup (dynamic_collision_hit):
    // last system to be executed
    // simply destroys the collision hit

struct player {
    e : id
    lives : int
    // probably other stuff here
}  

struct enemy {
    e : id 
    health : int
    // probably other stuff here
}

struct player_attack {
    damage : int 
    e : id
}

system player_collision_event_handler(player) {
    for every dynamic_collision_hit on player.id {
        if(collision.id contains "enemy") {
            //code here to lose life and respawn
        }
        // other cases here, like hitting items
    }
}

system enemy_health_system(enemy){
    // kind of same idea here as for handling player collisions
}

# static collisions and gravity




