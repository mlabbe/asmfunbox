# ASM Funbox #

This is the ASM Funbox, a stupid-simple game framework that lets you write your tick function in x64 Assembly.

If you are a game programmer looking to learn 64-bit assembly, this is a good approach.  The build system is extensible -- see Creating Modules below.

## Compiling ##

### Dependencies ###

 - Python for scons
 - scons for building
 - nasm for assembling
 - 64-bit GCC for compiling
 - SDL2 and OpenGL headers

### Known Issues ###

In `SConstruct`, `add_sdl_to_env` must be investigated, as the locations for `CPPPATH` and `LIBPATH` will need to be updated.

## Controls ##

See `sample_inputs` in `game.c`

## Creating Modules ##

Create a subdir under `src/modules`.  Copy `src/modules/stub/SConscript` into your directory, replacing `module_files` with a list of the files you want to assemble into a static library.

Build the module with:
    scons debug=1 module=modulename install

If successful, this places the module at `bin/modulename`.

Modules have one API responsibility: to implement `int asm_tick( Uint8 buttons, Uint32 *pixels, Uint32 elapsed_ms )`.

 - buttons is a bitflag of inputs which are either on or off.  See `BTN_` constants in main.h
 - pixels is a 32-bit RGBA buffer which represents your backbuffer for the frame.  It is `(BUFFER_W * BUFFER_H * 4)` bytes large.  This is uploaded to an OpenGL texture and displayed on a Window.
 - elapsed_ms is the number of milliseconds that have elapsed since program start.

## Author ##

Michael Labbe <mike+funbox@frogtoss.com>

