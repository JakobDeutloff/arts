# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

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
CMAKE_SOURCE_DIR = /home/richard/Work/arts/3rdparty/wigner

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/richard/Work/arts/3rdparty/wigner

# Include any dependencies generated for this target.
include wigxjpf/CMakeFiles/wigner.dir/depend.make

# Include the progress variables for this target.
include wigxjpf/CMakeFiles/wigner.dir/progress.make

# Include the compile flags for this target's objects.
include wigxjpf/CMakeFiles/wigner.dir/flags.make

wigxjpf/CMakeFiles/wigner.dir/src/prime_factor.o: wigxjpf/CMakeFiles/wigner.dir/flags.make
wigxjpf/CMakeFiles/wigner.dir/src/prime_factor.o: wigxjpf/src/prime_factor.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/richard/Work/arts/3rdparty/wigner/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object wigxjpf/CMakeFiles/wigner.dir/src/prime_factor.o"
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/wigner.dir/src/prime_factor.o   -c /home/richard/Work/arts/3rdparty/wigner/wigxjpf/src/prime_factor.c

wigxjpf/CMakeFiles/wigner.dir/src/prime_factor.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/wigner.dir/src/prime_factor.i"
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/richard/Work/arts/3rdparty/wigner/wigxjpf/src/prime_factor.c > CMakeFiles/wigner.dir/src/prime_factor.i

wigxjpf/CMakeFiles/wigner.dir/src/prime_factor.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/wigner.dir/src/prime_factor.s"
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/richard/Work/arts/3rdparty/wigner/wigxjpf/src/prime_factor.c -o CMakeFiles/wigner.dir/src/prime_factor.s

wigxjpf/CMakeFiles/wigner.dir/src/prime_factor.o.requires:

.PHONY : wigxjpf/CMakeFiles/wigner.dir/src/prime_factor.o.requires

wigxjpf/CMakeFiles/wigner.dir/src/prime_factor.o.provides: wigxjpf/CMakeFiles/wigner.dir/src/prime_factor.o.requires
	$(MAKE) -f wigxjpf/CMakeFiles/wigner.dir/build.make wigxjpf/CMakeFiles/wigner.dir/src/prime_factor.o.provides.build
.PHONY : wigxjpf/CMakeFiles/wigner.dir/src/prime_factor.o.provides

wigxjpf/CMakeFiles/wigner.dir/src/prime_factor.o.provides.build: wigxjpf/CMakeFiles/wigner.dir/src/prime_factor.o


wigxjpf/CMakeFiles/wigner.dir/src/calc.o: wigxjpf/CMakeFiles/wigner.dir/flags.make
wigxjpf/CMakeFiles/wigner.dir/src/calc.o: wigxjpf/src/calc.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/richard/Work/arts/3rdparty/wigner/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object wigxjpf/CMakeFiles/wigner.dir/src/calc.o"
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/wigner.dir/src/calc.o   -c /home/richard/Work/arts/3rdparty/wigner/wigxjpf/src/calc.c

wigxjpf/CMakeFiles/wigner.dir/src/calc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/wigner.dir/src/calc.i"
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/richard/Work/arts/3rdparty/wigner/wigxjpf/src/calc.c > CMakeFiles/wigner.dir/src/calc.i

wigxjpf/CMakeFiles/wigner.dir/src/calc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/wigner.dir/src/calc.s"
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/richard/Work/arts/3rdparty/wigner/wigxjpf/src/calc.c -o CMakeFiles/wigner.dir/src/calc.s

wigxjpf/CMakeFiles/wigner.dir/src/calc.o.requires:

.PHONY : wigxjpf/CMakeFiles/wigner.dir/src/calc.o.requires

wigxjpf/CMakeFiles/wigner.dir/src/calc.o.provides: wigxjpf/CMakeFiles/wigner.dir/src/calc.o.requires
	$(MAKE) -f wigxjpf/CMakeFiles/wigner.dir/build.make wigxjpf/CMakeFiles/wigner.dir/src/calc.o.provides.build
.PHONY : wigxjpf/CMakeFiles/wigner.dir/src/calc.o.provides

wigxjpf/CMakeFiles/wigner.dir/src/calc.o.provides.build: wigxjpf/CMakeFiles/wigner.dir/src/calc.o


wigxjpf/CMakeFiles/wigner.dir/src/trivial_zero.o: wigxjpf/CMakeFiles/wigner.dir/flags.make
wigxjpf/CMakeFiles/wigner.dir/src/trivial_zero.o: wigxjpf/src/trivial_zero.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/richard/Work/arts/3rdparty/wigner/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object wigxjpf/CMakeFiles/wigner.dir/src/trivial_zero.o"
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/wigner.dir/src/trivial_zero.o   -c /home/richard/Work/arts/3rdparty/wigner/wigxjpf/src/trivial_zero.c

wigxjpf/CMakeFiles/wigner.dir/src/trivial_zero.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/wigner.dir/src/trivial_zero.i"
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/richard/Work/arts/3rdparty/wigner/wigxjpf/src/trivial_zero.c > CMakeFiles/wigner.dir/src/trivial_zero.i

wigxjpf/CMakeFiles/wigner.dir/src/trivial_zero.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/wigner.dir/src/trivial_zero.s"
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/richard/Work/arts/3rdparty/wigner/wigxjpf/src/trivial_zero.c -o CMakeFiles/wigner.dir/src/trivial_zero.s

wigxjpf/CMakeFiles/wigner.dir/src/trivial_zero.o.requires:

.PHONY : wigxjpf/CMakeFiles/wigner.dir/src/trivial_zero.o.requires

wigxjpf/CMakeFiles/wigner.dir/src/trivial_zero.o.provides: wigxjpf/CMakeFiles/wigner.dir/src/trivial_zero.o.requires
	$(MAKE) -f wigxjpf/CMakeFiles/wigner.dir/build.make wigxjpf/CMakeFiles/wigner.dir/src/trivial_zero.o.provides.build
.PHONY : wigxjpf/CMakeFiles/wigner.dir/src/trivial_zero.o.provides

wigxjpf/CMakeFiles/wigner.dir/src/trivial_zero.o.provides.build: wigxjpf/CMakeFiles/wigner.dir/src/trivial_zero.o


wigxjpf/CMakeFiles/wigner.dir/src/c_wrap.o: wigxjpf/CMakeFiles/wigner.dir/flags.make
wigxjpf/CMakeFiles/wigner.dir/src/c_wrap.o: wigxjpf/src/c_wrap.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/richard/Work/arts/3rdparty/wigner/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object wigxjpf/CMakeFiles/wigner.dir/src/c_wrap.o"
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/wigner.dir/src/c_wrap.o   -c /home/richard/Work/arts/3rdparty/wigner/wigxjpf/src/c_wrap.c

wigxjpf/CMakeFiles/wigner.dir/src/c_wrap.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/wigner.dir/src/c_wrap.i"
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/richard/Work/arts/3rdparty/wigner/wigxjpf/src/c_wrap.c > CMakeFiles/wigner.dir/src/c_wrap.i

wigxjpf/CMakeFiles/wigner.dir/src/c_wrap.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/wigner.dir/src/c_wrap.s"
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/richard/Work/arts/3rdparty/wigner/wigxjpf/src/c_wrap.c -o CMakeFiles/wigner.dir/src/c_wrap.s

wigxjpf/CMakeFiles/wigner.dir/src/c_wrap.o.requires:

.PHONY : wigxjpf/CMakeFiles/wigner.dir/src/c_wrap.o.requires

wigxjpf/CMakeFiles/wigner.dir/src/c_wrap.o.provides: wigxjpf/CMakeFiles/wigner.dir/src/c_wrap.o.requires
	$(MAKE) -f wigxjpf/CMakeFiles/wigner.dir/build.make wigxjpf/CMakeFiles/wigner.dir/src/c_wrap.o.provides.build
.PHONY : wigxjpf/CMakeFiles/wigner.dir/src/c_wrap.o.provides

wigxjpf/CMakeFiles/wigner.dir/src/c_wrap.o.provides.build: wigxjpf/CMakeFiles/wigner.dir/src/c_wrap.o


wigxjpf/CMakeFiles/wigner.dir/src/fortran_wrap.o: wigxjpf/CMakeFiles/wigner.dir/flags.make
wigxjpf/CMakeFiles/wigner.dir/src/fortran_wrap.o: wigxjpf/src/fortran_wrap.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/richard/Work/arts/3rdparty/wigner/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object wigxjpf/CMakeFiles/wigner.dir/src/fortran_wrap.o"
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/wigner.dir/src/fortran_wrap.o   -c /home/richard/Work/arts/3rdparty/wigner/wigxjpf/src/fortran_wrap.c

wigxjpf/CMakeFiles/wigner.dir/src/fortran_wrap.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/wigner.dir/src/fortran_wrap.i"
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/richard/Work/arts/3rdparty/wigner/wigxjpf/src/fortran_wrap.c > CMakeFiles/wigner.dir/src/fortran_wrap.i

wigxjpf/CMakeFiles/wigner.dir/src/fortran_wrap.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/wigner.dir/src/fortran_wrap.s"
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/richard/Work/arts/3rdparty/wigner/wigxjpf/src/fortran_wrap.c -o CMakeFiles/wigner.dir/src/fortran_wrap.s

wigxjpf/CMakeFiles/wigner.dir/src/fortran_wrap.o.requires:

.PHONY : wigxjpf/CMakeFiles/wigner.dir/src/fortran_wrap.o.requires

wigxjpf/CMakeFiles/wigner.dir/src/fortran_wrap.o.provides: wigxjpf/CMakeFiles/wigner.dir/src/fortran_wrap.o.requires
	$(MAKE) -f wigxjpf/CMakeFiles/wigner.dir/build.make wigxjpf/CMakeFiles/wigner.dir/src/fortran_wrap.o.provides.build
.PHONY : wigxjpf/CMakeFiles/wigner.dir/src/fortran_wrap.o.provides

wigxjpf/CMakeFiles/wigner.dir/src/fortran_wrap.o.provides.build: wigxjpf/CMakeFiles/wigner.dir/src/fortran_wrap.o


# Object files for target wigner
wigner_OBJECTS = \
"CMakeFiles/wigner.dir/src/prime_factor.o" \
"CMakeFiles/wigner.dir/src/calc.o" \
"CMakeFiles/wigner.dir/src/trivial_zero.o" \
"CMakeFiles/wigner.dir/src/c_wrap.o" \
"CMakeFiles/wigner.dir/src/fortran_wrap.o"

# External object files for target wigner
wigner_EXTERNAL_OBJECTS =

wigxjpf/libwigner.a: wigxjpf/CMakeFiles/wigner.dir/src/prime_factor.o
wigxjpf/libwigner.a: wigxjpf/CMakeFiles/wigner.dir/src/calc.o
wigxjpf/libwigner.a: wigxjpf/CMakeFiles/wigner.dir/src/trivial_zero.o
wigxjpf/libwigner.a: wigxjpf/CMakeFiles/wigner.dir/src/c_wrap.o
wigxjpf/libwigner.a: wigxjpf/CMakeFiles/wigner.dir/src/fortran_wrap.o
wigxjpf/libwigner.a: wigxjpf/CMakeFiles/wigner.dir/build.make
wigxjpf/libwigner.a: wigxjpf/CMakeFiles/wigner.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/richard/Work/arts/3rdparty/wigner/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking C static library libwigner.a"
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && $(CMAKE_COMMAND) -P CMakeFiles/wigner.dir/cmake_clean_target.cmake
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/wigner.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
wigxjpf/CMakeFiles/wigner.dir/build: wigxjpf/libwigner.a

.PHONY : wigxjpf/CMakeFiles/wigner.dir/build

wigxjpf/CMakeFiles/wigner.dir/requires: wigxjpf/CMakeFiles/wigner.dir/src/prime_factor.o.requires
wigxjpf/CMakeFiles/wigner.dir/requires: wigxjpf/CMakeFiles/wigner.dir/src/calc.o.requires
wigxjpf/CMakeFiles/wigner.dir/requires: wigxjpf/CMakeFiles/wigner.dir/src/trivial_zero.o.requires
wigxjpf/CMakeFiles/wigner.dir/requires: wigxjpf/CMakeFiles/wigner.dir/src/c_wrap.o.requires
wigxjpf/CMakeFiles/wigner.dir/requires: wigxjpf/CMakeFiles/wigner.dir/src/fortran_wrap.o.requires

.PHONY : wigxjpf/CMakeFiles/wigner.dir/requires

wigxjpf/CMakeFiles/wigner.dir/clean:
	cd /home/richard/Work/arts/3rdparty/wigner/wigxjpf && $(CMAKE_COMMAND) -P CMakeFiles/wigner.dir/cmake_clean.cmake
.PHONY : wigxjpf/CMakeFiles/wigner.dir/clean

wigxjpf/CMakeFiles/wigner.dir/depend:
	cd /home/richard/Work/arts/3rdparty/wigner && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/richard/Work/arts/3rdparty/wigner /home/richard/Work/arts/3rdparty/wigner/wigxjpf /home/richard/Work/arts/3rdparty/wigner /home/richard/Work/arts/3rdparty/wigner/wigxjpf /home/richard/Work/arts/3rdparty/wigner/wigxjpf/CMakeFiles/wigner.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : wigxjpf/CMakeFiles/wigner.dir/depend

