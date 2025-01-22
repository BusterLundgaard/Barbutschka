**D** means "is coded, but hasn't actually been tested yet"

# MORE CURRENT ISSUES:
-[x] Write the systems definitions in the new components file
-[x] Write the add/remove queue in the new ecs_m
-[x] Write the update function in the new ecs_m
-[x] Actually write the new components_list
-[x] Get the shit refactored in different files and get it to run 


-[] Make collision adjustment with the static environment work
-[] Make collision adjustment with "stairs" in the staic environment work / diagonal slopes

# LONG TERM:
-[x] Backup "old code" somewhere else
-[x] Setup git for the project
-[X] Make it so that "update" function prints every time it calls a system like:
"Velocity_system(Transform[1], Velocity[1])"
where the numbers are how many are in the list, so these are lists of one element.
-[X] Add property to component that specifies if it is possible to have multiple on a single entity, enforce a crash/exception if you try to add multiple.

-[x] Add collision component
-[X] Create system for drawing collisions
-[X] Create the collision hit algorithm
-[X] Test collision hit algorithm with other collider examples

-[x] Refactor so that we use floats for every position instead of ints

-[X] Create system to draw the level

-[X] Update ECS manager to be able to handle singletons
-[X] Create and test collision hit algorithm with static terrain
-[X] Test that checking for collision events in a basic player charachter without gravity works well enough

-[] Refactor the component class so that instead of storing the type as a string, we store it as a type_index. Then they won't need to give all these functions the types explicitly if they just pass in a component! And in fact, you can make a general "typecasting" function that just takes a component and gives you the correct type out of it! Might be able to infer what it should return given 
context like auto t = get_sibling<_Transform>(comp)
-[] Refactor so that by writing just, for example __Level you refer to a global static variable with value typeid(_Level).

-[] Create SolidCollision component
-[] Create the collision adjustment algorithm
    See if it can somehow work so that we get moving platforms for free!
-[] Create entity with SolidCollision that you can move around with arrow keys in large chunks
-[] Check and fix the collision adjustment algorithm against static terrain
-[] Check and fix the collision adjustment against dynamic objects (use a system of "weight" where we assume only one moves the other)

-[] Create the basic movement of the player charachter

-[] Figure out moving platform dynamics

-[] Figure out system for going from room to room and keeping track of permanent world events

-[] Figure out a simple but flexible system for animations 

-[] Polish up main charachter movement

-[] Add attack. Test it against very simple "enemy" that is just a red square (ie, test the enemy loses health if hit, and that you loose health if walking into enemy)

-[] Figure out moving platforms
