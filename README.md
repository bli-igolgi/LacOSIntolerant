# Executing
To run the OS
 - First rename long ass _lac.... folder name to mp3
 - Make the kernel using `sudo make` inside student-distrib
 - Open the test_debug by running `windows_run.bat` on the windows machine
    - You cannot run this inside of QEMU
 - Run the script to target the test_debug machine by running `gdb -x test_os.sh` on the devel machine

# ECE391 MP3 - Package contents

#### createfs
This program takes a flat source directory (i.e. no subdirectories
in the source directory) and creates a filesystem image in the
format specified for this MP.  Run it with no parameters to see
usage.

#### elfconvert
This program takes a 32-bit ELF (Executable and Linking Format) file - the standard executable type on Linux - and converts it to the
executable format specified for this MP.  The output filename is <exename>.converted.

#### fish/
This directory contains the source for the fish animation program.
It can be compiled two ways - one for your operating system, and one
for Linux using an emulation layer.  The Makefile is currently set
up to build "fish" for your operating system using the elfconvert
utility described above.  If you want to build a Linux version, do
"make fish_emulated".  You can then run fish_emulated as superuser
at a standard Linux console, and you should see the fish animation.

#### fsdir/
This is the directory from which your filesystem image was created.
It contains versions of cat, fish, grep, hello, ls, and shell, as
well as the frame0.txt and frame1.txt files that fish needs to run.
If you want to change files in your OS's filesystem, modify this
directory and then run the "createfs" utility on it to create a new
filesystem image.

#### README
This file.

#### student-distrib/
This is the directory that contains the source code for your
operating system.  Currently, a skeleton is provided that will build
and boot you into protected mode, printing out various boot
parameters.  Read the INSTALL file in that directory for
instructions on how to set up the bootloader to boot this OS.

#### syscalls/
This directory contains a basic system call library that is used by
the utility programs such as cat, grep, ls, etc.  The library
provides a C interface to the system calls, much like the C library
(libc) provides on a real Linux/Unix system.  A few support
functions have also been written (things like strlen, strcpy, etc.)
that are used by the utility programs.  The Makefile is set up to
build these programs for your OS.