#pragma once
/*
  asmfunbox Copyright (C) 2014-2023 Frogtoss Games, Inc.
*/


typedef struct {
    SDL_Window*   window;
    SDL_GLContext gl_context;
    SDL_Surface*  back;
    GLuint        back_handle;
} global_state_t;

extern global_state_t g_state; /* main.c */

void game_loop(void);
