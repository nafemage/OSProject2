# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /opt/software/software/CMake/3.16.4-GCCcore-9.3.0/bin/cmake

# The command to remove a file.
RM = /opt/software/software/CMake/3.16.4-GCCcore-9.3.0/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /homes/hpfannen/hw2/OSProject2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /homes/hpfannen/hw2/OSProject2/build

# Include any dependencies generated for this target.
include CMakeFiles/hw2_test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/hw2_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/hw2_test.dir/flags.make

CMakeFiles/hw2_test.dir/test/tests.cpp.o: CMakeFiles/hw2_test.dir/flags.make
CMakeFiles/hw2_test.dir/test/tests.cpp.o: ../test/tests.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/homes/hpfannen/hw2/OSProject2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/hw2_test.dir/test/tests.cpp.o"
	/opt/software/software/GCCcore/9.3.0/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/hw2_test.dir/test/tests.cpp.o -c /homes/hpfannen/hw2/OSProject2/test/tests.cpp

CMakeFiles/hw2_test.dir/test/tests.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/hw2_test.dir/test/tests.cpp.i"
	/opt/software/software/GCCcore/9.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /homes/hpfannen/hw2/OSProject2/test/tests.cpp > CMakeFiles/hw2_test.dir/test/tests.cpp.i

CMakeFiles/hw2_test.dir/test/tests.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/hw2_test.dir/test/tests.cpp.s"
	/opt/software/software/GCCcore/9.3.0/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /homes/hpfannen/hw2/OSProject2/test/tests.cpp -o CMakeFiles/hw2_test.dir/test/tests.cpp.s

# Object files for target hw2_test
hw2_test_OBJECTS = \
"CMakeFiles/hw2_test.dir/test/tests.cpp.o"

# External object files for target hw2_test
hw2_test_EXTERNAL_OBJECTS =

hw2_test: CMakeFiles/hw2_test.dir/test/tests.cpp.o
hw2_test: CMakeFiles/hw2_test.dir/build.make
hw2_test: libdyn_array.a
hw2_test: libprocess_scheduling.a
hw2_test: libutilities.a
hw2_test: libprocess_scheduling.a
hw2_test: libdyn_array.a
hw2_test: CMakeFiles/hw2_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/homes/hpfannen/hw2/OSProject2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable hw2_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/hw2_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/hw2_test.dir/build: hw2_test

.PHONY : CMakeFiles/hw2_test.dir/build

CMakeFiles/hw2_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/hw2_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/hw2_test.dir/clean

CMakeFiles/hw2_test.dir/depend:
	cd /homes/hpfannen/hw2/OSProject2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /homes/hpfannen/hw2/OSProject2 /homes/hpfannen/hw2/OSProject2 /homes/hpfannen/hw2/OSProject2/build /homes/hpfannen/hw2/OSProject2/build /homes/hpfannen/hw2/OSProject2/build/CMakeFiles/hw2_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/hw2_test.dir/depend

