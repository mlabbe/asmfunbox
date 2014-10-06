/* GPL3 asm funbox */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL_opengl.h>
#include <GL/glu.h>

#include "main.h"
#include "game.h"


struct globals_s globals;

void print_fatal_sdl_error( char *str )
{
    fprintf( stderr, "%s", str );
    fprintf( stderr, ": %s\n", SDL_GetError() );
    exit(1);
}

void gl_sane_defaults( void )
{
    /* glob all gl state initialization into one function because program is simple. */
    glViewport( 0, 0, (GLsizei)WINDOW_W, (GLsizei)WINDOW_H );
    
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0.0, 1.0, 1.0, 0.0 );
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glEnable( GL_TEXTURE_2D );

    SDL_GL_SetSwapInterval( VSYNC );
}


int main(int argc, char *argv[])
{
    /* init internals */
    memset( &globals, 0, sizeof(struct globals_s) );

    /* init SDL */
    if ( SDL_Init( SDL_INIT_VIDEO ) != 0 )
        print_fatal_sdl_error( "SDL_Init" );
    atexit( SDL_Quit );

    globals.window = SDL_CreateWindow( "ASM Funbox",
                                       SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                       WINDOW_W, WINDOW_H,
                                       SDL_WINDOW_OPENGL );
    if ( !globals.window )
        print_fatal_sdl_error( "SDL_CreateWindow" );

    globals.glcontext = SDL_GL_CreateContext(globals.window);
    if ( !globals.glcontext )
        print_fatal_sdl_error( "SDL_GL_CreateContext" );

    globals.backbuf = SDL_CreateRGBSurface( 0,
                                            BUFFER_W, BUFFER_H,
                                            32, 0, 0, 0, 0 );
    if ( !globals.backbuf )
        print_fatal_sdl_error( "SDL_CreateRGBSurface" );

    /* init GL */
    gl_sane_defaults();

    /* temp debug crap */

    SDL_GL_SwapWindow( globals.window );

    glClearColor( 1.0f, 0.0f, 1.0f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT );
    SDL_GL_SwapWindow( globals.window );

    globals.timer.elapsed_ms = SDL_GetTicks();

    /* game.c */
    game_loop();

    // todo: cleanup surfaces and textures

    return 0;
}
