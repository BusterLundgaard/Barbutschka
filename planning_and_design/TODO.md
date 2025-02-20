**D** means "is coded, but hasn't actually been tested yet"

# MORE CURRENT ISSUES:
-[x] Write the systems definitions in the new components file
-[x] Write the add/remove queue in the new ecs_m
-[x] Write the update function in the new ecs_m
-[x] Actually write the new components_list
-[x] Get the shit refactored in different files and get it to run 
-[x] Potentially rewrite the whole collision system, it's a bloody unstable mess.
    -[x] I think dependency on "previous frame" is really bad 

-[] Make it so that you can set the width/height of individual animations and also set an offset
-[] Get basic walk and jump and idle animations working for grandma
-[] Get interaction with moving objects and slopes working properly for grandma 
-[] Get slap and thrust attack working (at least animation and movement wise)
-[] Create a simple enemy you can test attacks against. 

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

-[] Eventually you have to *also* add a more "static" collision system that just looks at position and not velocity
    This is unfortunately neccesary for rare cases very you might get stuck in blocks from the other system
    You've sort of already implemented it, at least for adjustables against solids. Now you just need it for adjustables against static terrain. 