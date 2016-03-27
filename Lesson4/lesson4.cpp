//
// Created by Silvio Fragnani da Silva on 20/03/16.
//
#include <iostream>
#include <GL/glew.h>
#include <SDL.h>
#include <Cleanup.h>
#include <Timer.h>

using namespace std;

// GL vars
GLuint gProgramId = 0;
GLuint gVAO1 = 0;
GLuint gVAO2 = 0;

// game loop vars
bool quit = false;
SDL_Event event;

int countedFrames = 1;
Timer fpsTimer;

bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cout << "SDL_Init error " << SDL_GetError() << endl;
        return false;
    }
    return true;
}

void setOpenGLVersion() {
    // set GL version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE); //SDL_GL_CONTEXT_PROFILE_COMPATIBILITY
}

SDL_Window *createSDLWindow() {
    SDL_Window *window = SDL_CreateWindow("SDL / OpenGL",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          600, 600,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (window == nullptr) {
        cout << "SDL_CreateWindow error " << SDL_GetError() << endl;
        SDL_Quit();
        return nullptr;
    }

    return window;
}

SDL_GLContext initSDLGLContext(SDL_Window *window) {
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (glContext == nullptr) {
        cout << "SDL_GL_CreateContext error " << SDL_GetError() << endl;
        cleanup(window);
        SDL_Quit();
        return nullptr;
    }

    // SDL_GL_SetSwapInterval(0) set immediate swap (high FPS)
    if (SDL_GL_SetSwapInterval(1) != 0) {
        cout << "Warning: unable to set VSync. Error " << SDL_GetError() << endl;
    }

    return glContext;
}

bool initGLEW(SDL_Window *window) {
    GLenum error;
    glewExperimental = GL_TRUE;
    error = glewInit();
    if (GLEW_OK != error) {
        cout << "GLEWInit error: " << glewGetErrorString(error) << endl;
        cleanup(window);
        SDL_Quit();
        return false;
    }
    return true;
}

void printShaderLog(GLuint shader) {
    // make sure name is shader
    if (glIsShader(shader)) {
        // shader log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        // get info string length
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // allocate string
        char *infoLog = new char[maxLength];

        // get info log
        glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
        if (infoLogLength > 0) {
            cout << infoLog << endl;
        }

        // deallocate string
        delete[] infoLog;

    } else {
        cout << "Name " << shader << " is not a shader" << endl;
    }
}

void printProgramLog(GLuint program) {
    // make sure name is shader
    if (glIsProgram(program)) {
        // program log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        // get info string length
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // allocate string
        char *infoLog = new char[maxLength];

        // get info log
        glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
        if (infoLogLength > 0) {
            // print Log
            cout << infoLog << endl;
        }

        // deallocate string
        delete[] infoLog;
    } else {
        cout << "Name " << program << " is not a program" << endl;
    }
}

bool initGLStructure() {
    // create program id
    gProgramId = glCreateProgram();

    //////////////////
    // VERTEX SHADER
    //////////////////
    // create vertex shader
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);

    // vertex shader source
    const GLchar *vertexShaderSource[] = {
            "#version 400\nin vec3 vp; void main() { gl_Position = vec4(vp, 1.0); }"
    };

    // set vertex shader source
    glShaderSource(vertexShaderId, 1, vertexShaderSource, NULL);

    // compile vertex shader
    glCompileShader(vertexShaderId);

    // check vertex shader error
    GLint shaderCompiled = GL_FALSE;
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &shaderCompiled);
    if (shaderCompiled != GL_TRUE) {
        cout << "Unable to compile vertex shader\n" << vertexShaderId << endl;
        printShaderLog(vertexShaderId);
        return false;
    }

    // attach vertex shader to program
    glAttachShader(gProgramId, vertexShaderId);

    ////////////////////
    // FRAGMENT SHADER
    ////////////////////
    // create fragment shader id
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // get fragment shader source
    const GLchar *fragmentShaderSource[]{
            "#version 400\nout vec4 frag_colour; void main() { frag_colour = vec4(0.0, 1.0, 0.0, 1.0); }"
    };

    // set fragment shader source
    glShaderSource(fragmentShaderId, 1, fragmentShaderSource, NULL);

    // compile fragment shader
    glCompileShader(fragmentShaderId);

    // check fragment shader errors
    shaderCompiled = GL_FALSE;
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &shaderCompiled);
    if (shaderCompiled != GL_TRUE) {
        cout << "Unable to compile fragment shader" << vertexShaderId << endl;
        printShaderLog(fragmentShaderId);
        return false;
    }

    // attach fragment shader
    glAttachShader(gProgramId, fragmentShaderId);

    // link program
    glLinkProgram(gProgramId);

    // check program errors
    GLint programSuccess = GL_TRUE;
    glGetProgramiv(gProgramId, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        cout << "Error linking program " << gProgramId << endl;
        printProgramLog(gProgramId);
        return false;
    }

    return true;
}

void loadGlData() {
    // Vertex Buffer Object (VBO) Data
    GLfloat vertexData1[] = {
            0.0f, 0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f
    };

    GLfloat vertexData2[] = {
            0.7f, 0.7f, 0.0f,
            0.9f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.0f
    };

    GLuint gVBO1, gVBO2;
    // create VBO1
    glGenBuffers(1, &gVBO1);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO1);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertexData1, GL_STATIC_DRAW);

    // create VAO1
    glGenVertexArrays(1, &gVAO1);
    glBindVertexArray(gVAO1);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // create VBO2
    glGenBuffers(1, &gVBO2);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO2);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertexData2, GL_STATIC_DRAW);

    // create VAO2
    glGenVertexArrays(1, &gVAO2);
    glBindVertexArray(gVAO2);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

void eventHandler() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
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
                default:
                    break;
            }
        }
    }
}

void calculatePrintFps() {
    int ticks = fpsTimer.getTicks();
    if (countedFrames != 0 && ticks != 0) {
        // calculate and correct fps
        float avgFPS = countedFrames / (ticks / 1000.f);
        countedFrames++;

        int ticksModule = ticks % 1000;
        if (ticksModule >= 0 && ticksModule <= 12) {
            cout << "FPS " << avgFPS << endl;
        }
    }
}

void render() {
    // initialize clear color
    glClearColor(0.f, 0.f, 1.f, 1.f);
    // wipe the drawing surface clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind program
    glUseProgram(gProgramId);
    glBindVertexArray(gVAO1);
    // draw points 0-3 from the currently bound VAO with current in-use shader
    glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

    glBindVertexArray(gVAO2);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

    // unbind program
    glUseProgram(NULL);
}

void printVersions() {
    cout << "----------------------------------------------------------------" << endl;
    cout << "Graphics Successfully Initialized" << endl;
    cout << "OpenGL Info" << endl;
    cout << "    Version: " << glGetString(GL_VERSION) << endl;
    cout << "     Vendor: " << glGetString(GL_VENDOR) << endl;
    cout << "   Renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "    Shading: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    cout << "       GLEW: " << glewGetString(GLEW_VERSION) << endl;
    cout << "----------------------------------------------------------------" << endl;
}

int main() {
    if (!initSDL()) {
        return 1;
    }

    setOpenGLVersion();

    SDL_Window *window = createSDLWindow();
    if (window == nullptr) {
        return 1;
    }

    SDL_GLContext glContext = initSDLGLContext(window);
    if (glContext == nullptr) {
        return 1;
    }

    if (!initGLEW(window)) {
        return 1;
    }

    if (!initGLStructure()) {
        return 1;
    }

    loadGlData();

    printVersions();

    fpsTimer.start();

    while (!quit) {
        eventHandler();
        calculatePrintFps();
        render();
        SDL_GL_SwapWindow(window);
    }

    // clean up everything
    cleanup(&glContext, window);
    SDL_Quit();

    return 0;
}

