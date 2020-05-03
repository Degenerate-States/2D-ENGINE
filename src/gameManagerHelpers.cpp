#include "gameManager.h"
#include "config.h"
#include <iostream>
#include <glad/glad.h>
#include <tuple>
using namespace std;


tuple<SDL_Window*,SDL_Renderer*> GameManager::SDL_Visuals_Boilerplate(Config* cfg){
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL2 video subsystem couldn't be initialized. Error: "
                  << SDL_GetError()
                  << std::endl;
        exit(1);
    }

    SDL_Window* window = SDL_CreateWindow("Glad Sample",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          cfg->windowSizeX, cfg->windowSizeY, SDL_WINDOW_SHOWN |
                                          SDL_WINDOW_OPENGL);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED);
    
    if (renderer == nullptr) {
        std::cerr << "SDL2 Renderer couldn't be created. Error: "
                  << SDL_GetError()
                  << std::endl;
        exit(1);
    }

    // Create a OpenGL context on SDL2
    this->gl_context = SDL_GL_CreateContext(window);
    
    // Load GL extensions using glad
    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize the OpenGL context." << std::endl;
        exit(1);
    }

    // Loaded OpenGL successfully.
    std::cout << "OpenGL version loaded: " << GLVersion.major << "."
              << GLVersion.minor << std::endl;

    //sets clear color
    glClearColor(0, 0, 0, 1.0f);

    //line smoothing
    glEnable( GL_LINE_SMOOTH );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return make_tuple(window,renderer);
}