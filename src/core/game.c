
#include <stdio.h>
#include <SDL.h>
#include <math.h>

#include "main.h"
#include "game.h"


static void surface_to_display( SDL_Surface *surface, GLuint *texture_handle )
{
    /* perf: use glTexSubImage2D */
    if ( *texture_handle ) {
        glDeleteTextures( 1, texture_handle );
    }

    glGenTextures( 1, texture_handle );
    glBindTexture( GL_TEXTURE_2D, *texture_handle );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    SDL_LockSurface( surface );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, BUFFER_W, BUFFER_H,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
    SDL_UnlockSurface( surface );

    GLfloat w = 1.0f;
    GLfloat h = 1.0f;
    glBegin(GL_QUADS);

    glTexCoord2f( 0.0f, 0.0f );
    glVertex2f( 0.0f, 0.0f );

    glTexCoord2f( 0.0f, 1.0f );
    glVertex2f( 0.0f, h );
    
    glTexCoord2f( 1.0f, 1.0f );
    glVertex2f( w, h );

    glTexCoord2f( 1.0f, 0.0f );
    glVertex2f( w, 0.0f );

    glEnd();
    
}

static void sample_inputs( int *quit, struct input_s *input )
{
    SDL_Event event;
    Uint8 *buttons = &input->buttons;

#define SCANCODE event.key.keysym.scancode

    while ( SDL_PollEvent(&event) ) {
        switch ( event.type ) {
        case SDL_KEYUP:
            if ( SCANCODE == SDL_SCANCODE_ESCAPE )
                *quit = 1;

            if ( SCANCODE == SDL_SCANCODE_LEFT )
                *buttons &= ~BTN_LEFT;

            if ( SCANCODE == SDL_SCANCODE_RIGHT )
                *buttons &= ~BTN_RIGHT;

            if ( SCANCODE == SDL_SCANCODE_UP )
                *buttons &= ~BTN_UP;

            if ( SCANCODE == SDL_SCANCODE_DOWN )
                *buttons &= ~BTN_DOWN;

            if ( SCANCODE == SDL_SCANCODE_1 )
                *buttons &= ~BTN_START;

            if ( SCANCODE == SDL_SCANCODE_A )
                *buttons &= ~BTN_A;

            if ( SCANCODE == SDL_SCANCODE_B )
                *buttons &= ~BTN_B;
            
        break;

        case SDL_KEYDOWN:
            if ( SCANCODE == SDL_SCANCODE_LEFT )
                *buttons |= BTN_LEFT;

            if ( SCANCODE == SDL_SCANCODE_RIGHT )
                *buttons |= BTN_RIGHT;

            if ( SCANCODE == SDL_SCANCODE_UP )
                *buttons |= BTN_UP;

            if ( SCANCODE == SDL_SCANCODE_DOWN )
                *buttons |= BTN_DOWN;

            if ( SCANCODE == SDL_SCANCODE_1 )
                *buttons |= BTN_START;

            if ( SCANCODE == SDL_SCANCODE_A )
                *buttons |= BTN_A;

            if ( SCANCODE == SDL_SCANCODE_B )
                *buttons |= BTN_B;
        break;
            
        }
    }

#undef SCANCODE
}

static void update_timer( struct timer_s *timer )
{
    Uint32 new_elapsed_ms = SDL_GetTicks();
    timer->frame_delta = new_elapsed_ms - timer->elapsed_ms;
    timer->elapsed_ms = new_elapsed_ms;

    if ( timer->frame_delta >= FRAME_COMPLAIN_THRESHOLD ) {        
        printf("long frame: %i ms\n", timer->elapsed_ms );
    }
}

#if 0 
static void game_update( struct input_s *input )
{
}

static void game_render( SDL_Surface *surface )
{
    SDL_LockSurface( surface );
    Uint32 *px = (Uint32*)surface->pixels;
    Uint16 x,y;

    /* just do some silly stuff for now to prove frame updating is in. */
    for ( y = 0; y < BUFFER_H; ++y )
    {
        for ( x = 0; x < BUFFER_W; ++x )
        {
            float fr,fg,fb;
            
            fr = (float)x/BUFFER_W;
            fg = (float)y/BUFFER_H;
            fb = (float)(x+y)/ sqrtf(BUFFER_W+BUFFER_H);

            Uint8 r = (Uint8)(fr*255.0f)  | globals.timer.elapsed_ms;
            Uint8 g = (Uint8)(fg*255.0f)  + ~globals.timer.elapsed_ms;
            Uint8 b = (Uint8)((Uint32)(fb*255.0f) % globals.timer.elapsed_ms);
            
            *px = SDL_MapRGB( surface->format, r, g, b );
            px++;
        }
    }

    SDL_UnlockSurface( surface );
}
#endif


extern int asm_tick( Uint8 buttons, Uint32 *pixels, Uint32 elapsed_ms );

static void game_tick( int *quit, struct input_s *input, SDL_Surface *surface, struct timer_s *timer )
{
    *quit |= asm_tick( input->buttons, (Uint32*)surface->pixels, timer->elapsed_ms );
}


void game_loop( void )
{
    int quit = 0;
    while ( !quit ) {
        
        update_timer( &globals.timer );
        sample_inputs( &quit, &globals.input );

        game_tick( &quit, &globals.input, globals.backbuf, &globals.timer );
#if 0
        game_update( &globals.input );
        game_render( globals.backbuf );
#endif

        surface_to_display( globals.backbuf, &globals.backbuf_handle );
        SDL_GL_SwapWindow( globals.window );
    }
}
