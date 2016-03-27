#include <iostream>
#include <SDL.h>
#include "Cleanup.h"
#include "Timer.h"

using namespace std;

int main() {
    // init SDL sistem
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cout << "SDL_Init error" << SDL_GetError() << endl;
        return 1;
    }

    // creating a window
    SDL_Window *window = SDL_CreateWindow("Lesson 1",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          500, 500,
                                          SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        cout << "SDL_CreateWindow error " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    // creating a render
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); //SDL_RENDERER_PRESENTVSYNC
    if (renderer == nullptr) {
        cleanup(window);
        cout << "SDL_CreateRenderer error " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    // loading image
    char *basePath = SDL_GetBasePath();
    string imagePath = basePath;
    imagePath += "hello.bmp";

    SDL_Surface *surface = SDL_LoadBMP(imagePath.c_str());
    if (surface == nullptr) {
        cleanup(renderer, window);
        cout << "SDL_LoadBMP error " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    // creating texture from surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (texture == nullptr) {
        cleanup(renderer, window);
        cout << "SDL_CreateTextureFromSurface error " << SDL_GetError() << endl;
        SDL_Quit();
    }

    //The frames per second
    const int FRAMES_PER_SECOND = 60;
    const int SCREEN_TICKS_PER_FRAME = 1000 / FRAMES_PER_SECOND;

    //Keep track of the current countedFrames
    int countedFrames = 0;
    //Whether or not to cap the countedFrames rate
    bool cap = true;
    //The frames per second timer
    Timer fpsTimer;
    //The frames per second cap timer
    Timer capTimer;

    fpsTimer.start();
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        //Start cap timer
        capTimer.start();

        // check user events
        while (SDL_PollEvent(&event)) {
            // close application when user click at x on windows
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            // close application when user press q on keyboard
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_q) {
                quit = true;
            }

            // Switch CAP
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                //Switch cap
                cap = (!cap);
            }
        }

        // Drawing the image at the window
        // first clean up the renderer
        SDL_RenderClear(renderer);
        // draw the texture
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        // update the screen
        SDL_RenderPresent(renderer);


        //Calculate and correct fps
        float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
//        if (avgFPS > 200) {
//            avgFPS = 0;
//        }
        cout << "FPS " << avgFPS << endl;

        //Increment the countedFrames counter
        countedFrames++;

        //If we want to cap the countedFrames rate
        int frameTicks = capTimer.getTicks();
        if (cap && (frameTicks < SCREEN_TICKS_PER_FRAME)) {
            //Sleep the remaining countedFrames time
            SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
        }
    }

    // Clean up everything
    cleanup(texture, renderer, window);
    SDL_Quit();

    return 0;
}