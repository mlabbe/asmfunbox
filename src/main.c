/*
  asmfunbox Copyright (C) 2014-2023 Frogtoss Games, Inc.
*/

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <GL/glu.h>

#include "limits.h"
#include "game.h"

global_state_t g_state;


static void
fatal_sdl_error(char* str)
{
    fprintf(stderr, "%s", str);
    fprintf(stderr, ": %s\n", SDL_GetError());
    exit(1);
}

static void
gl_sane_defaults(void)
{
    /* glob all gl state initialization into one function because program is simple. */
    glViewport(0, 0, (GLsizei)WINDOW_W, (GLsizei)WINDOW_H);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 1.0, 1.0, 0.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    // SDL_GL_SetSwapInterval(VSYNC);
}


int
main(void)
{
    //
    // startup
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0)
        fatal_sdl_error("SDL_Init");
    atexit(SDL_Quit);

    g_state.window = SDL_CreateWindow("ASM Funbox",
                                      SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      WINDOW_W,
                                      WINDOW_H,
                                      SDL_WINDOW_OPENGL);

    if (!g_state.window)
        fatal_sdl_error("SDL_CreateWindow");

    g_state.gl_context = SDL_GL_CreateContext(g_state.window);
    if (!g_state.gl_context)
        fatal_sdl_error("SDL_GL_CreateContext");

    g_state.back = SDL_CreateRGBSurface(0, BUFFER_W, BUFFER_H, 32, 0, 0, 0, 0);
    if (!g_state.back)
        fatal_sdl_error("SDL_CreateRGBSurface");

    gl_sane_defaults();

    //
    // execute

    game_loop();

    //
    // shutdown
    SDL_FreeSurface(g_state.back);
    SDL_DestroyWindow(g_state.window);


    return 0;
}
