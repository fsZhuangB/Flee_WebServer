# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ubuntu/RemoteWorking/Flee_WebServer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ubuntu/RemoteWorking/Flee_WebServer/build

# Include any dependencies generated for this target.
include CMakeFiles/myserver.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/myserver.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/myserver.dir/flags.make

CMakeFiles/myserver.dir/main.cpp.o: CMakeFiles/myserver.dir/flags.make
CMakeFiles/myserver.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/RemoteWorking/Flee_WebServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/myserver.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/myserver.dir/main.cpp.o -c /home/ubuntu/RemoteWorking/Flee_WebServer/main.cpp

CMakeFiles/myserver.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myserver.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/RemoteWorking/Flee_WebServer/main.cpp > CMakeFiles/myserver.dir/main.cpp.i

CMakeFiles/myserver.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myserver.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/RemoteWorking/Flee_WebServer/main.cpp -o CMakeFiles/myserver.dir/main.cpp.s

CMakeFiles/myserver.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/myserver.dir/main.cpp.o.requires

CMakeFiles/myserver.dir/main.cpp.o.provides: CMakeFiles/myserver.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/myserver.dir/build.make CMakeFiles/myserver.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/myserver.dir/main.cpp.o.provides

CMakeFiles/myserver.dir/main.cpp.o.provides.build: CMakeFiles/myserver.dir/main.cpp.o


CMakeFiles/myserver.dir/webserver.cpp.o: CMakeFiles/myserver.dir/flags.make
CMakeFiles/myserver.dir/webserver.cpp.o: ../webserver.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/RemoteWorking/Flee_WebServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/myserver.dir/webserver.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/myserver.dir/webserver.cpp.o -c /home/ubuntu/RemoteWorking/Flee_WebServer/webserver.cpp

CMakeFiles/myserver.dir/webserver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/myserver.dir/webserver.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/RemoteWorking/Flee_WebServer/webserver.cpp > CMakeFiles/myserver.dir/webserver.cpp.i

CMakeFiles/myserver.dir/webserver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/myserver.dir/webserver.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/RemoteWorking/Flee_WebServer/webserver.cpp -o CMakeFiles/myserver.dir/webserver.cpp.s

CMakeFiles/myserver.dir/webserver.cpp.o.requires:

.PHONY : CMakeFiles/myserver.dir/webserver.cpp.o.requires

CMakeFiles/myserver.dir/webserver.cpp.o.provides: CMakeFiles/myserver.dir/webserver.cpp.o.requires
	$(MAKE) -f CMakeFiles/myserver.dir/build.make CMakeFiles/myserver.dir/webserver.cpp.o.provides.build
.PHONY : CMakeFiles/myserver.dir/webserver.cpp.o.provides

CMakeFiles/myserver.dir/webserver.cpp.o.provides.build: CMakeFiles/myserver.dir/webserver.cpp.o


# Object files for target myserver
myserver_OBJECTS = \
"CMakeFiles/myserver.dir/main.cpp.o" \
"CMakeFiles/myserver.dir/webserver.cpp.o"

# External object files for target myserver
myserver_EXTERNAL_OBJECTS =

myserver: CMakeFiles/myserver.dir/main.cpp.o
myserver: CMakeFiles/myserver.dir/webserver.cpp.o
myserver: CMakeFiles/myserver.dir/build.make
myserver: http/libhttp.a
myserver: threadpool/libthreadpool.a
myserver: CMakeFiles/myserver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ubuntu/RemoteWorking/Flee_WebServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable myserver"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/myserver.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/myserver.dir/build: myserver

.PHONY : CMakeFiles/myserver.dir/build

CMakeFiles/myserver.dir/requires: CMakeFiles/myserver.dir/main.cpp.o.requires
CMakeFiles/myserver.dir/requires: CMakeFiles/myserver.dir/webserver.cpp.o.requires

.PHONY : CMakeFiles/myserver.dir/requires

CMakeFiles/myserver.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/myserver.dir/cmake_clean.cmake
.PHONY : CMakeFiles/myserver.dir/clean

CMakeFiles/myserver.dir/depend:
	cd /home/ubuntu/RemoteWorking/Flee_WebServer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/RemoteWorking/Flee_WebServer /home/ubuntu/RemoteWorking/Flee_WebServer /home/ubuntu/RemoteWorking/Flee_WebServer/build /home/ubuntu/RemoteWorking/Flee_WebServer/build /home/ubuntu/RemoteWorking/Flee_WebServer/build/CMakeFiles/myserver.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/myserver.dir/depend

