cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories for headers
include_directories(code)

# Define the executable
add_executable(main
    main.cpp
    code/input.cpp
    code/component_list.cpp
    code/ecs_m.cpp 
    code/system.cpp
    code/component.cpp
    code/systems/position_and_collision.cpp 
    code/systems/player.cpp 
    code/systems/moving_things.cpp 
    code/systems/drawing.cpp 
    
)

# Link libraries
find_package(raylib REQUIRED) # Find raylib library
target_link_libraries(main PRIVATE raylib)

# Optional: Enable warnings during compilation
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(main PRIVATE -w) # Suppress all warnings
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(main PRIVATE /w) # Suppress all warnings (MSVC)
endif()