# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.26

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
CMAKE_COMMAND = /home/linuxbrew/.linuxbrew/Cellar/cmake/3.26.2/bin/cmake

# The command to remove a file.
RM = /home/linuxbrew/.linuxbrew/Cellar/cmake/3.26.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dede/Code/Github/sipeto

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dede/Code/Github/sipeto

# Include any dependencies generated for this target.
include CMakeFiles/sipeto.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/sipeto.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/sipeto.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/sipeto.dir/flags.make

CMakeFiles/sipeto.dir/src/main.cpp.o: CMakeFiles/sipeto.dir/flags.make
CMakeFiles/sipeto.dir/src/main.cpp.o: src/main.cpp
CMakeFiles/sipeto.dir/src/main.cpp.o: CMakeFiles/sipeto.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dede/Code/Github/sipeto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/sipeto.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sipeto.dir/src/main.cpp.o -MF CMakeFiles/sipeto.dir/src/main.cpp.o.d -o CMakeFiles/sipeto.dir/src/main.cpp.o -c /home/dede/Code/Github/sipeto/src/main.cpp

CMakeFiles/sipeto.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sipeto.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dede/Code/Github/sipeto/src/main.cpp > CMakeFiles/sipeto.dir/src/main.cpp.i

CMakeFiles/sipeto.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sipeto.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dede/Code/Github/sipeto/src/main.cpp -o CMakeFiles/sipeto.dir/src/main.cpp.s

CMakeFiles/sipeto.dir/src/sipeto.cpp.o: CMakeFiles/sipeto.dir/flags.make
CMakeFiles/sipeto.dir/src/sipeto.cpp.o: src/sipeto.cpp
CMakeFiles/sipeto.dir/src/sipeto.cpp.o: CMakeFiles/sipeto.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dede/Code/Github/sipeto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/sipeto.dir/src/sipeto.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sipeto.dir/src/sipeto.cpp.o -MF CMakeFiles/sipeto.dir/src/sipeto.cpp.o.d -o CMakeFiles/sipeto.dir/src/sipeto.cpp.o -c /home/dede/Code/Github/sipeto/src/sipeto.cpp

CMakeFiles/sipeto.dir/src/sipeto.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sipeto.dir/src/sipeto.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dede/Code/Github/sipeto/src/sipeto.cpp > CMakeFiles/sipeto.dir/src/sipeto.cpp.i

CMakeFiles/sipeto.dir/src/sipeto.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sipeto.dir/src/sipeto.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dede/Code/Github/sipeto/src/sipeto.cpp -o CMakeFiles/sipeto.dir/src/sipeto.cpp.s

CMakeFiles/sipeto.dir/src/simple_http_server.cpp.o: CMakeFiles/sipeto.dir/flags.make
CMakeFiles/sipeto.dir/src/simple_http_server.cpp.o: src/simple_http_server.cpp
CMakeFiles/sipeto.dir/src/simple_http_server.cpp.o: CMakeFiles/sipeto.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dede/Code/Github/sipeto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/sipeto.dir/src/simple_http_server.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sipeto.dir/src/simple_http_server.cpp.o -MF CMakeFiles/sipeto.dir/src/simple_http_server.cpp.o.d -o CMakeFiles/sipeto.dir/src/simple_http_server.cpp.o -c /home/dede/Code/Github/sipeto/src/simple_http_server.cpp

CMakeFiles/sipeto.dir/src/simple_http_server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sipeto.dir/src/simple_http_server.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dede/Code/Github/sipeto/src/simple_http_server.cpp > CMakeFiles/sipeto.dir/src/simple_http_server.cpp.i

CMakeFiles/sipeto.dir/src/simple_http_server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sipeto.dir/src/simple_http_server.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dede/Code/Github/sipeto/src/simple_http_server.cpp -o CMakeFiles/sipeto.dir/src/simple_http_server.cpp.s

# Object files for target sipeto
sipeto_OBJECTS = \
"CMakeFiles/sipeto.dir/src/main.cpp.o" \
"CMakeFiles/sipeto.dir/src/sipeto.cpp.o" \
"CMakeFiles/sipeto.dir/src/simple_http_server.cpp.o"

# External object files for target sipeto
sipeto_EXTERNAL_OBJECTS =

sipeto: CMakeFiles/sipeto.dir/src/main.cpp.o
sipeto: CMakeFiles/sipeto.dir/src/sipeto.cpp.o
sipeto: CMakeFiles/sipeto.dir/src/simple_http_server.cpp.o
sipeto: CMakeFiles/sipeto.dir/build.make
sipeto: /home/linuxbrew/.linuxbrew/lib/libspdlog.so.1.11.0
sipeto: /home/linuxbrew/.linuxbrew/lib/libboost_system-mt.so.1.81.0
sipeto: /home/linuxbrew/.linuxbrew/lib/libboost_thread-mt.so.1.81.0
sipeto: /home/linuxbrew/.linuxbrew/Cellar/curl/8.0.1/lib/libcurl.so
sipeto: /home/linuxbrew/.linuxbrew/lib/libfmt.so.9.1.0
sipeto: CMakeFiles/sipeto.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dede/Code/Github/sipeto/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable sipeto"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sipeto.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/sipeto.dir/build: sipeto
.PHONY : CMakeFiles/sipeto.dir/build

CMakeFiles/sipeto.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/sipeto.dir/cmake_clean.cmake
.PHONY : CMakeFiles/sipeto.dir/clean

CMakeFiles/sipeto.dir/depend:
	cd /home/dede/Code/Github/sipeto && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dede/Code/Github/sipeto /home/dede/Code/Github/sipeto /home/dede/Code/Github/sipeto /home/dede/Code/Github/sipeto /home/dede/Code/Github/sipeto/CMakeFiles/sipeto.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/sipeto.dir/depend
