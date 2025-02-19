**D** means "is coded, but hasn't actually been tested yet"

# MORE CURRENT ISSUES:
-[x] Write the systems definitions in the new components file
-[x] Write the add/remove queue in the new ecs_m
-[x] Write the update function in the new ecs_m
-[x] Actually write the new components_list
-[x] Get the shit refactored in different files and get it to run 
-[x] Potentially rewrite the whole collision system, it's a bloody unstable mess.
    -[x] I think dependency on "previous frame" is really bad 

-[] Plot the idea for the jump arc in geogebra so that you can actually tweak parameters and test against what your system computes. 

-[] Add support in the ComponentList for having more than MaxElements elements??
-[] Change the collision system so that it works on layers, where you specify a matrix of which layers interact
    -[] Make a document/file for "Project settings" like these where it's all just constants 

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

-[] Eventually you have to *also* add a more "static" collision system that just looks at position and not velocity
    This is unfortunately neccesary for rare cases very you might get stuck in blocks from the other system
    You've sort of already implemented it, at least for adjustables against solids. Now you just need it for adjustables against static terrain. 