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
CMAKE_SOURCE_DIR = /homes/hpfannen/hw2-repo-test/OSProject2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /homes/hpfannen/hw2-repo-test/OSProject2/build

# Include any dependencies generated for this target.
include CMakeFiles/process_scheduling.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/process_scheduling.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/process_scheduling.dir/flags.make

CMakeFiles/process_scheduling.dir/src/process_scheduling.c.o: CMakeFiles/process_scheduling.dir/flags.make
CMakeFiles/process_scheduling.dir/src/process_scheduling.c.o: ../src/process_scheduling.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/homes/hpfannen/hw2-repo-test/OSProject2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/process_scheduling.dir/src/process_scheduling.c.o"
	/opt/software/software/GCCcore/9.3.0/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/process_scheduling.dir/src/process_scheduling.c.o   -c /homes/hpfannen/hw2-repo-test/OSProject2/src/process_scheduling.c

CMakeFiles/process_scheduling.dir/src/process_scheduling.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/process_scheduling.dir/src/process_scheduling.c.i"
	/opt/software/software/GCCcore/9.3.0/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /homes/hpfannen/hw2-repo-test/OSProject2/src/process_scheduling.c > CMakeFiles/process_scheduling.dir/src/process_scheduling.c.i

CMakeFiles/process_scheduling.dir/src/process_scheduling.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/process_scheduling.dir/src/process_scheduling.c.s"
	/opt/software/software/GCCcore/9.3.0/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /homes/hpfannen/hw2-repo-test/OSProject2/src/process_scheduling.c -o CMakeFiles/process_scheduling.dir/src/process_scheduling.c.s

# Object files for target process_scheduling
process_scheduling_OBJECTS = \
"CMakeFiles/process_scheduling.dir/src/process_scheduling.c.o"

# External object files for target process_scheduling
process_scheduling_EXTERNAL_OBJECTS =

libprocess_scheduling.a: CMakeFiles/process_scheduling.dir/src/process_scheduling.c.o
libprocess_scheduling.a: CMakeFiles/process_scheduling.dir/build.make
libprocess_scheduling.a: CMakeFiles/process_scheduling.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/homes/hpfannen/hw2-repo-test/OSProject2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libprocess_scheduling.a"
	$(CMAKE_COMMAND) -P CMakeFiles/process_scheduling.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/process_scheduling.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/process_scheduling.dir/build: libprocess_scheduling.a

.PHONY : CMakeFiles/process_scheduling.dir/build

CMakeFiles/process_scheduling.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/process_scheduling.dir/cmake_clean.cmake
.PHONY : CMakeFiles/process_scheduling.dir/clean

CMakeFiles/process_scheduling.dir/depend:
	cd /homes/hpfannen/hw2-repo-test/OSProject2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /homes/hpfannen/hw2-repo-test/OSProject2 /homes/hpfannen/hw2-repo-test/OSProject2 /homes/hpfannen/hw2-repo-test/OSProject2/build /homes/hpfannen/hw2-repo-test/OSProject2/build /homes/hpfannen/hw2-repo-test/OSProject2/build/CMakeFiles/process_scheduling.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/process_scheduling.dir/depend

