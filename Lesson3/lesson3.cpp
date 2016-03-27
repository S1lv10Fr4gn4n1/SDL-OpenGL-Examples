//
// Created by Silvio Fragnani da Silva on 20/03/16.
//
#include <iostream>
#include <GL/glew.h>
#include <SDL.h>
#include <Cleanup.h>
#include <Timer.h>

using namespace std;

// http://gafferongames.com/game-physics/fix-your-timestep/
struct State {
    float x;
    float v;
};

struct Derivative {
    float dx;
    float dv;
};

State interpolate(const State &previous, const State &current, float alpha) {
    State state;
    state.x = current.x * alpha + previous.x * (1 - alpha);
    state.v = current.v * alpha + previous.v * (1 - alpha);
    return state;
}

float acceleration(const State &state, float t) {
    const float k = 10;
    const float b = 1;
    return -k * state.x - b * state.v;
}

Derivative evaluate(const State &initial, float t) {
    Derivative output;
    output.dx = initial.v;
    output.dv = acceleration(initial, t);
    return output;
}

Derivative evaluate(const State &initial, float t, float dt, const Derivative &d) {
    State state;
    state.x = initial.x + d.dx * dt;
    state.v = initial.v + d.dv * dt;
    Derivative output;
    output.dx = state.v;
    output.dv = acceleration(state, t + dt);
    return output;
}

void integrate(State &state, float t, float dt) {
    Derivative a = evaluate(state, t);
    Derivative b = evaluate(state, t, dt * 0.5f, a);
    Derivative c = evaluate(state, t, dt * 0.5f, b);
    Derivative d = evaluate(state, t, dt, c);

    const float dxdt = 1.0f / 6.0f * (a.dx + 2.0f * (b.dx + c.dx) + d.dx);
    const float dvdt = 1.0f / 6.0f * (a.dv + 2.0f * (b.dv + c.dv) + d.dv);

    state.x = state.x + dxdt * dt;
    state.v = state.v + dvdt * dt;
}

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

    // set immediate swap (high FPS)
    SDL_GL_SetSwapInterval(0);
//    // sync with vertical trace
//    if (SDL_GL_SetSwapInterval(1) != 0) {
//        cout << "Warning: unable to set VSync. Error " << SDL_GetError() << endl;
//    }

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

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(4);

    glClearColor(0.3f, 0.3f, 0.3f, 1);

    int countedFrames = 1;
    Timer fpsTimer;
    fpsTimer.start();

    State current;
    current.x = 1;
    current.v = 0;

    State previous = current;

    float t = 0.0f;
    float dt = 0.1f;

    float currentTime = 0.0f;
    float accumulator = 0.0f;

    SDL_Event event;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (event.key.keysym.sym == SDLK_r) {
                accumulator = 0.0f;
                current.x = 100;
                current.v = 0;
                previous = current;
            }

            if (event.type == SDL_WINDOWEVENT) {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_SHOWN:
                        SDL_Log("Window %d shown", event.window.windowID);
                        break;
                    case SDL_WINDOWEVENT_HIDDEN:
                        SDL_Log("Window %d hidden", event.window.windowID);
                        break;
                    case SDL_WINDOWEVENT_RESTORED:
                        SDL_Log("Window %d restored", event.window.windowID);
                        break;

                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        countedFrames = 1;
                        fpsTimer.start();

                        break;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        fpsTimer.stop();
                        break;
                }
            }
        }

        if (countedFrames != 0 && fpsTimer.getTicks() != 0) {
            //Calculate and correct fps
            float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
            cout << "FPS " << avgFPS << endl;
            countedFrames++;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const float newTime = SDL_GetTicks();
        float deltaTime = newTime - currentTime;
        currentTime = newTime;

        if (deltaTime > 0.25f)
            deltaTime = 0.25f;

        accumulator += deltaTime / 2;

        while (accumulator >= dt) {
            accumulator -= dt;
            previous = current;
            integrate(current, t, dt);
            t += dt;
        }

        State state = interpolate(previous, current, accumulator / dt);

        glBegin(GL_POINTS);
        glColor3f(1, 1, 1);
        glVertex3f(state.x, 0, 0);
        glEnd();

        SDL_GL_SwapWindow(window);
    }

    // Clean up everything
    cleanup(&glContext, window);
    SDL_Quit();

    return 0;
}
