-[] Backup "old code" somewhere else
-[] Setup git for the project
-[] Make it so that "update" function prints every time it calls a system like:
"Velocity_system(Transform[1], Velocity[1])"
where the numbers are how many are in the list, so these are lists of one element.
-[] Add property to component that specifies if it is possible to have multiple on a single entity, enforce a crash/exception if you try to add multiple.

-[] Add collision component
-[] Create system for drawing collisions
-[?] Create the collision hit algorithm
-[] Test collision hit algorithm with other collider examples

-[] Create system to draw the level

-[] Test collision hit algorithm with static terrain
-[] Test that checking for collision events in a basic player charachter without gravity works well enough

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
