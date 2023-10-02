# ASM Funbox #

This is the ASM Funbox.  Giving you input and a framebuffer 60 times a second, write game logic in x64 assembly.

Explore writing 64-bit assembly by writing a game without all of the setup or shutdown calls, focusing on pure game logic.

## Major Update ##

This is ASM Funbox 2.0, which removes SCons and makes other quality of life updates.  For legacy projects, ASM Funbox 1.0 is still available at the tag `asmfunbox_1` but is no longer supported.

## Compiling ##

### Dependencies ###

ASM Funbox runs on Linux.  Any VM that can run a fixed function OpenGL
pipeline should be sufficient, in lieu of dedicated hardware.

 - Python for scons
 - nasm for assembling
 - SDL2 and OpenGL headers
 
`apt-get install nasm libsdl2-dev libglu1-mesa-dev` should do it.

### Known Issues ###

None. Report bugs!


## Controls ##

See `sample_input_buttons` in `game.c`

## Building ##

After installing dependencies (see above):

    ./make.py
    
This produces executables in the bin subdirectory.

## Writing Your Own Game ##

Each file in `src/asm` generates an exe with the same name under the `bin` directory. So, create `src/asm/game.asm` to generate `bin/game`.

The game must implement a function called `asm_tick`.  See `stub.asm` for the simplest example.

The prototype of `asm_tick` is, effectively:

    int asm_tick(u8 buttons, uint32 *pixels, uint32 elapsed_ms);
    
 - If asm_tick returns 1, the program quits. 

 - `buttons` is a set of bitflags, akin to a NES controller.  See `BTN` constants in `game.c`.

 - `pixels` is a 160x90 32-bit RGBA image buffer.  Write your pixels once per frame.
 
 - `elapsed_ms` is the number of elapsed milliseconds since program start

## Author ##

[Michael Labbe](https://michaellabbe.com)

