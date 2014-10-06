#ifndef MAIN_H
#define MAIN_H

#include <SDL.h>
#include <GL/glu.h>

/* size of actual rendering window */
#define WINDOW_W 1920
#define WINDOW_H 1080

/* size of pixel buffer to stretch over target rendering window */
#define BUFFER_W 320/2
#define BUFFER_H (int)((float)(WINDOW_W/WINDOW_H)*BUFFER_W) /* match window aspect ratio */

#define VSYNC 0

#define BTN_LEFT  (1<<1)
#define BTN_RIGHT (1<<2)
#define BTN_UP    (1<<3)
#define BTN_DOWN  (1<<4)
#define BTN_START (1<<5)
#define BTN_A     (1<<6)
#define BTN_B     (1<<7)

struct input_s {
    Uint8 buttons;
};

struct timer_s {
    Uint32 frame_delta;
    Uint32 elapsed_ms;
};

struct globals_s {
    SDL_Window *window;
    SDL_Surface *backbuf;
    SDL_GLContext glcontext;
    
    struct input_s input;
    struct timer_s timer;
    GLuint backbuf_handle;
};


extern struct globals_s globals;

#endif 
