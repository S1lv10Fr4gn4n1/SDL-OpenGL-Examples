//
// Created by Silvio Fragnani da Silva on 20/03/16.
//
#include <iostream>
#include <GL/glew.h>
#include <SDL.h>
#include "Cleanup.h"

using namespace std;

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cout << "SDL_Init error " << SDL_GetError() << endl;
        return 1;
    }

    // set GL version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // create window
    SDL_Window *window = SDL_CreateWindow("SDL with OpenGL",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          500, 500,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    // check if window was created
    if (window == nullptr) {
        cout << "SDL_CreateWindow error " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    // create GL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (glContext == nullptr) {
        cout << "SDL_GL_CreateContext error " << SDL_GetError() << endl;
        cleanup(window);
        SDL_Quit();
        return 1;
    }

    if (SDL_GL_SetSwapInterval(1) != 0) {
        cout << "Warning: unable to set VSync. Error " << SDL_GetError() << endl;
    }


    //////////////////////////////////
    // initialize GLEW
    //////////////////////////////////
    GLenum error = GL_NO_ERROR;
    error = glewInit();
    if (GLEW_OK != error) {
        cleanup(window);
        cout << "glewInit error: " << glewGetErrorString(error) << endl;
        return 1;
    }

    //////////////////////////////////
    // initialize OpenGL
    //////////////////////////////////
    // initialize projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // check for errors
    error = glGetError();
    if (error != GL_NO_ERROR) {
        cout << "Error initializing OpenGL! " << glewGetErrorString(error) << endl;
        cleanup(&glContext, window);
        return 1;
    }

    // initialize Modelview Matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // check for errors
    error = glGetError();
    if (error != GL_NO_ERROR) {
        cout << "Error initializing OpenGL! " << glewGetErrorString(error) << endl;
        cleanup(&glContext, window);
        return 1;
    }

    SDL_Event event;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // clear color
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // drawing a square
        glBegin(GL_QUADS);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.5f, 0.5f);
        glVertex2f(-0.5f, 0.5f);
        glEnd();

        SDL_GL_SwapWindow(window);
    }

    return 0;
}
