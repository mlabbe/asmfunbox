/*
  asmfunbox Copyright (C) 2014-2023 Frogtoss Games, Inc.
*/

#include <stdint.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>

#include "game.h"
#include "limits.h"


typedef struct {
    uint32_t frame_delta;
    uint32_t elapsed_ms;
} game_timer_t;

// BTN_* bitflags
typedef uint8_t buttons_t;

#define BTN_LEFT (1)
#define BTN_RIGHT (1 << 1)
#define BTN_UP (1 << 2)
#define BTN_DOWN (1 << 3)
#define BTN_SELECT (1 << 4)
#define BTN_START (1 << 5)
#define BTN_A (1 << 6)
#define BTN_B (1 << 7)


// this is the only function that needs to be implemented in assembly
extern int asm_tick(buttons_t buttons, uint32_t* pixels, uint32_t elapsed_ms);

// set this to true to not call into asm at all -- for debugging, only
#define IMPLEMENT_GAME_IN_C 0

static void
surface_to_display(SDL_Surface* surface, GLuint* texture_handle)
{
    /* perf: use glTexSubImage2D */
    if (*texture_handle) {
        glDeleteTextures(1, texture_handle);
    }

    glGenTextures(1, texture_handle);
    glBindTexture(GL_TEXTURE_2D, *texture_handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    SDL_LockSurface(surface);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 BUFFER_W,
                 BUFFER_H,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 surface->pixels);
    SDL_UnlockSurface(surface);

    GLfloat w = 1.0f;
    GLfloat h = 1.0f;
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0.0f, h);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(w, h);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(w, 0.0f);

    glEnd();
}

static void
update_timer(game_timer_t* t)
{
    uint32_t new_elapsed_ms = SDL_GetTicks();
    t->frame_delta = new_elapsed_ms - t->elapsed_ms;
    t->elapsed_ms = new_elapsed_ms;

    if (t->frame_delta >= FRAME_COMPLAIN_THRESHOLD) {
        printf("long frame: %i ms\n", t->elapsed_ms);
    }
}

static void
sample_input_buttons(int* quit, buttons_t* buttons)
{
    SDL_Event event;

#define SCANCODE event.key.keysym.scancode

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYUP:
            if (SCANCODE == SDL_SCANCODE_ESCAPE)
                *quit = 1;

            if (SCANCODE == SDL_SCANCODE_LEFT)
                *buttons &= ~BTN_LEFT;

            if (SCANCODE == SDL_SCANCODE_RIGHT)
                *buttons &= ~BTN_RIGHT;

            if (SCANCODE == SDL_SCANCODE_UP)
                *buttons &= ~BTN_UP;

            if (SCANCODE == SDL_SCANCODE_DOWN)
                *buttons &= ~BTN_DOWN;

            if (SCANCODE == SDL_SCANCODE_1)
                *buttons &= ~BTN_START;

            if (SCANCODE == SDL_SCANCODE_2)
                *buttons &= ~BTN_SELECT;

            if (SCANCODE == SDL_SCANCODE_A)
                *buttons &= ~BTN_A;

            if (SCANCODE == SDL_SCANCODE_B)
                *buttons &= ~BTN_B;

            break;

        case SDL_KEYDOWN:
            if (SCANCODE == SDL_SCANCODE_LEFT)
                *buttons |= BTN_LEFT;

            if (SCANCODE == SDL_SCANCODE_RIGHT)
                *buttons |= BTN_RIGHT;

            if (SCANCODE == SDL_SCANCODE_UP)
                *buttons |= BTN_UP;

            if (SCANCODE == SDL_SCANCODE_DOWN)
                *buttons |= BTN_DOWN;

            if (SCANCODE == SDL_SCANCODE_1)
                *buttons |= BTN_START;

            if (SCANCODE == SDL_SCANCODE_2)
                *buttons |= BTN_SELECT;

            if (SCANCODE == SDL_SCANCODE_A)
                *buttons |= BTN_A;

            if (SCANCODE == SDL_SCANCODE_B)
                *buttons |= BTN_B;
            break;
        }
    }

#undef SCANCODE
}

#if IMPLEMENT_GAME_IN_C
static void
c_tick(buttons_t buttons, uint32_t* px, uint32_t elapsed_ms, SDL_PixelFormat* pixel_format)
{
    Uint16 x, y;

    /* just do some silly stuff for now to prove frame updating is in. */
    for (y = 0; y < BUFFER_H; ++y) {
        for (x = 0; x < BUFFER_W; ++x) {
            float fr, fg, fb;

            fr = (float)x / BUFFER_W;
            fg = (float)y / BUFFER_H;
            fb = (float)(x + y) / sqrtf(BUFFER_W + BUFFER_H);

            Uint8 r = (Uint8)(fr * 255.0f) | elapsed_ms;
            Uint8 g = (Uint8)(fg * 255.0f) + ~elapsed_ms;
            Uint8 b = (Uint8)((Uint32)(fb * 255.0f) % elapsed_ms);

            *px = SDL_MapRGB(pixel_format, r, g, b);
            px++;
        }
    }
}

#endif

static void
game_tick(int* quit, buttons_t* buttons, SDL_Surface* surf, game_timer_t* timer)
{
#if IMPLEMENT_GAME_IN_C
    SDL_LockSurface(surf->pixels);
    c_tick(*buttons, (uint32_t*)surf->pixels, timer->elapsed_ms, surf->format);
    SDL_UnlockSurface(surf->pixels);
#else
    SDL_LockSurface(surf->pixels);
    asm_tick(*buttons, (uint32_t*)surf->pixels, timer->elapsed_ms);
    SDL_UnlockSurface(surf->pixels);
#endif
}


void
game_loop(void)
{
    game_timer_t timer = {0};

    int     quit = 0;
    uint8_t buttons = 0;

    while (!quit) {
        update_timer(&timer);
        sample_input_buttons(&quit, &buttons);

        game_tick(&quit, &buttons, g_state.back, &timer);

        surface_to_display(g_state.back, &g_state.back_handle);
        SDL_GL_SwapWindow(g_state.window);
    }
}
