# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/buster/Desktop/Bartbushka

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/buster/Desktop/Bartbushka/build

# Include any dependencies generated for this target.
include CMakeFiles/main.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/main.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/main.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/main.dir/flags.make

CMakeFiles/main.dir/codegen:
.PHONY : CMakeFiles/main.dir/codegen

CMakeFiles/main.dir/main.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/main.cpp.o: /home/buster/Desktop/Bartbushka/main.cpp
CMakeFiles/main.dir/main.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/buster/Desktop/Bartbushka/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/main.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/main.cpp.o -MF CMakeFiles/main.dir/main.cpp.o.d -o CMakeFiles/main.dir/main.cpp.o -c /home/buster/Desktop/Bartbushka/main.cpp

CMakeFiles/main.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/buster/Desktop/Bartbushka/main.cpp > CMakeFiles/main.dir/main.cpp.i

CMakeFiles/main.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/buster/Desktop/Bartbushka/main.cpp -o CMakeFiles/main.dir/main.cpp.s

CMakeFiles/main.dir/code/input.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/code/input.cpp.o: /home/buster/Desktop/Bartbushka/code/input.cpp
CMakeFiles/main.dir/code/input.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/buster/Desktop/Bartbushka/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/main.dir/code/input.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/code/input.cpp.o -MF CMakeFiles/main.dir/code/input.cpp.o.d -o CMakeFiles/main.dir/code/input.cpp.o -c /home/buster/Desktop/Bartbushka/code/input.cpp

CMakeFiles/main.dir/code/input.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/code/input.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/buster/Desktop/Bartbushka/code/input.cpp > CMakeFiles/main.dir/code/input.cpp.i

CMakeFiles/main.dir/code/input.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/code/input.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/buster/Desktop/Bartbushka/code/input.cpp -o CMakeFiles/main.dir/code/input.cpp.s

CMakeFiles/main.dir/code/component_list.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/code/component_list.cpp.o: /home/buster/Desktop/Bartbushka/code/component_list.cpp
CMakeFiles/main.dir/code/component_list.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/buster/Desktop/Bartbushka/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/main.dir/code/component_list.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/code/component_list.cpp.o -MF CMakeFiles/main.dir/code/component_list.cpp.o.d -o CMakeFiles/main.dir/code/component_list.cpp.o -c /home/buster/Desktop/Bartbushka/code/component_list.cpp

CMakeFiles/main.dir/code/component_list.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/code/component_list.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/buster/Desktop/Bartbushka/code/component_list.cpp > CMakeFiles/main.dir/code/component_list.cpp.i

CMakeFiles/main.dir/code/component_list.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/code/component_list.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/buster/Desktop/Bartbushka/code/component_list.cpp -o CMakeFiles/main.dir/code/component_list.cpp.s

CMakeFiles/main.dir/code/ecs_m.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/code/ecs_m.cpp.o: /home/buster/Desktop/Bartbushka/code/ecs_m.cpp
CMakeFiles/main.dir/code/ecs_m.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/buster/Desktop/Bartbushka/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/main.dir/code/ecs_m.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/code/ecs_m.cpp.o -MF CMakeFiles/main.dir/code/ecs_m.cpp.o.d -o CMakeFiles/main.dir/code/ecs_m.cpp.o -c /home/buster/Desktop/Bartbushka/code/ecs_m.cpp

CMakeFiles/main.dir/code/ecs_m.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/code/ecs_m.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/buster/Desktop/Bartbushka/code/ecs_m.cpp > CMakeFiles/main.dir/code/ecs_m.cpp.i

CMakeFiles/main.dir/code/ecs_m.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/code/ecs_m.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/buster/Desktop/Bartbushka/code/ecs_m.cpp -o CMakeFiles/main.dir/code/ecs_m.cpp.s

CMakeFiles/main.dir/code/system.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/code/system.cpp.o: /home/buster/Desktop/Bartbushka/code/system.cpp
CMakeFiles/main.dir/code/system.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/buster/Desktop/Bartbushka/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/main.dir/code/system.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/code/system.cpp.o -MF CMakeFiles/main.dir/code/system.cpp.o.d -o CMakeFiles/main.dir/code/system.cpp.o -c /home/buster/Desktop/Bartbushka/code/system.cpp

CMakeFiles/main.dir/code/system.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/code/system.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/buster/Desktop/Bartbushka/code/system.cpp > CMakeFiles/main.dir/code/system.cpp.i

CMakeFiles/main.dir/code/system.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/code/system.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/buster/Desktop/Bartbushka/code/system.cpp -o CMakeFiles/main.dir/code/system.cpp.s

CMakeFiles/main.dir/code/component.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/code/component.cpp.o: /home/buster/Desktop/Bartbushka/code/component.cpp
CMakeFiles/main.dir/code/component.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/buster/Desktop/Bartbushka/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/main.dir/code/component.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/code/component.cpp.o -MF CMakeFiles/main.dir/code/component.cpp.o.d -o CMakeFiles/main.dir/code/component.cpp.o -c /home/buster/Desktop/Bartbushka/code/component.cpp

CMakeFiles/main.dir/code/component.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/code/component.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/buster/Desktop/Bartbushka/code/component.cpp > CMakeFiles/main.dir/code/component.cpp.i

CMakeFiles/main.dir/code/component.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/code/component.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/buster/Desktop/Bartbushka/code/component.cpp -o CMakeFiles/main.dir/code/component.cpp.s

CMakeFiles/main.dir/code/systems/position_and_collision.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/code/systems/position_and_collision.cpp.o: /home/buster/Desktop/Bartbushka/code/systems/position_and_collision.cpp
CMakeFiles/main.dir/code/systems/position_and_collision.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/buster/Desktop/Bartbushka/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/main.dir/code/systems/position_and_collision.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/code/systems/position_and_collision.cpp.o -MF CMakeFiles/main.dir/code/systems/position_and_collision.cpp.o.d -o CMakeFiles/main.dir/code/systems/position_and_collision.cpp.o -c /home/buster/Desktop/Bartbushka/code/systems/position_and_collision.cpp

CMakeFiles/main.dir/code/systems/position_and_collision.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/code/systems/position_and_collision.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/buster/Desktop/Bartbushka/code/systems/position_and_collision.cpp > CMakeFiles/main.dir/code/systems/position_and_collision.cpp.i

CMakeFiles/main.dir/code/systems/position_and_collision.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/code/systems/position_and_collision.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/buster/Desktop/Bartbushka/code/systems/position_and_collision.cpp -o CMakeFiles/main.dir/code/systems/position_and_collision.cpp.s

CMakeFiles/main.dir/code/systems/player.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/code/systems/player.cpp.o: /home/buster/Desktop/Bartbushka/code/systems/player.cpp
CMakeFiles/main.dir/code/systems/player.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/buster/Desktop/Bartbushka/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/main.dir/code/systems/player.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/code/systems/player.cpp.o -MF CMakeFiles/main.dir/code/systems/player.cpp.o.d -o CMakeFiles/main.dir/code/systems/player.cpp.o -c /home/buster/Desktop/Bartbushka/code/systems/player.cpp

CMakeFiles/main.dir/code/systems/player.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/code/systems/player.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/buster/Desktop/Bartbushka/code/systems/player.cpp > CMakeFiles/main.dir/code/systems/player.cpp.i

CMakeFiles/main.dir/code/systems/player.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/code/systems/player.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/buster/Desktop/Bartbushka/code/systems/player.cpp -o CMakeFiles/main.dir/code/systems/player.cpp.s

CMakeFiles/main.dir/code/systems/moving_things.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/code/systems/moving_things.cpp.o: /home/buster/Desktop/Bartbushka/code/systems/moving_things.cpp
CMakeFiles/main.dir/code/systems/moving_things.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/buster/Desktop/Bartbushka/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/main.dir/code/systems/moving_things.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/code/systems/moving_things.cpp.o -MF CMakeFiles/main.dir/code/systems/moving_things.cpp.o.d -o CMakeFiles/main.dir/code/systems/moving_things.cpp.o -c /home/buster/Desktop/Bartbushka/code/systems/moving_things.cpp

CMakeFiles/main.dir/code/systems/moving_things.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/code/systems/moving_things.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/buster/Desktop/Bartbushka/code/systems/moving_things.cpp > CMakeFiles/main.dir/code/systems/moving_things.cpp.i

CMakeFiles/main.dir/code/systems/moving_things.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/code/systems/moving_things.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/buster/Desktop/Bartbushka/code/systems/moving_things.cpp -o CMakeFiles/main.dir/code/systems/moving_things.cpp.s

CMakeFiles/main.dir/code/systems/drawing.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/code/systems/drawing.cpp.o: /home/buster/Desktop/Bartbushka/code/systems/drawing.cpp
CMakeFiles/main.dir/code/systems/drawing.cpp.o: CMakeFiles/main.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/buster/Desktop/Bartbushka/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/main.dir/code/systems/drawing.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/main.dir/code/systems/drawing.cpp.o -MF CMakeFiles/main.dir/code/systems/drawing.cpp.o.d -o CMakeFiles/main.dir/code/systems/drawing.cpp.o -c /home/buster/Desktop/Bartbushka/code/systems/drawing.cpp

CMakeFiles/main.dir/code/systems/drawing.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/main.dir/code/systems/drawing.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/buster/Desktop/Bartbushka/code/systems/drawing.cpp > CMakeFiles/main.dir/code/systems/drawing.cpp.i

CMakeFiles/main.dir/code/systems/drawing.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/main.dir/code/systems/drawing.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/buster/Desktop/Bartbushka/code/systems/drawing.cpp -o CMakeFiles/main.dir/code/systems/drawing.cpp.s

# Object files for target main
main_OBJECTS = \
"CMakeFiles/main.dir/main.cpp.o" \
"CMakeFiles/main.dir/code/input.cpp.o" \
"CMakeFiles/main.dir/code/component_list.cpp.o" \
"CMakeFiles/main.dir/code/ecs_m.cpp.o" \
"CMakeFiles/main.dir/code/system.cpp.o" \
"CMakeFiles/main.dir/code/component.cpp.o" \
"CMakeFiles/main.dir/code/systems/position_and_collision.cpp.o" \
"CMakeFiles/main.dir/code/systems/player.cpp.o" \
"CMakeFiles/main.dir/code/systems/moving_things.cpp.o" \
"CMakeFiles/main.dir/code/systems/drawing.cpp.o"

# External object files for target main
main_EXTERNAL_OBJECTS =

main: CMakeFiles/main.dir/main.cpp.o
main: CMakeFiles/main.dir/code/input.cpp.o
main: CMakeFiles/main.dir/code/component_list.cpp.o
main: CMakeFiles/main.dir/code/ecs_m.cpp.o
main: CMakeFiles/main.dir/code/system.cpp.o
main: CMakeFiles/main.dir/code/component.cpp.o
main: CMakeFiles/main.dir/code/systems/position_and_collision.cpp.o
main: CMakeFiles/main.dir/code/systems/player.cpp.o
main: CMakeFiles/main.dir/code/systems/moving_things.cpp.o
main: CMakeFiles/main.dir/code/systems/drawing.cpp.o
main: CMakeFiles/main.dir/build.make
main: CMakeFiles/main.dir/compiler_depend.ts
main: /usr/lib/libraylib.so
main: CMakeFiles/main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/buster/Desktop/Bartbushka/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Linking CXX executable main"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/main.dir/build: main
.PHONY : CMakeFiles/main.dir/build

CMakeFiles/main.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/main.dir/cmake_clean.cmake
.PHONY : CMakeFiles/main.dir/clean

CMakeFiles/main.dir/depend:
	cd /home/buster/Desktop/Bartbushka/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/buster/Desktop/Bartbushka /home/buster/Desktop/Bartbushka /home/buster/Desktop/Bartbushka/build /home/buster/Desktop/Bartbushka/build /home/buster/Desktop/Bartbushka/build/CMakeFiles/main.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/main.dir/depend

