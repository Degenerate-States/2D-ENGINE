#include "gameManager.h"
#include "config.h"
#include <iostream>
#include <glad/glad.h>
#include <tuple>
using namespace std;

void GameManager::Check_Quit(){
    //events
    switch(this->event.type) {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_KEYDOWN:
            switch(this->event.key.keysym.sym) {
            case SDLK_ESCAPE:
                running = false;
                break;
            }
    }
}
void GameManager::fixFramerate(){
    this->tickNumber+=1;
    double frameDuration = SDL_GetTicks() -this->frameTime;

    double currentfps = 1000.0 / frameDuration;

    if (this->tickNumber%300 == 0){
        
        cout <<"fps: "<< currentfps <<  endl;
    }

    if (this->mspf > frameDuration){
        SDL_Delay(this->mspf - frameDuration);
    }
    this->frameTime = SDL_GetTicks();
}
tuple<SDL_Window*,SDL_Renderer*> GameManager::SDL_Visuals_Boilerplate(Config* cfg){
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "SDL2 video subsystem couldn't be initialized. Error: "
                  << SDL_GetError()
                  << endl;
        exit(1);
    }

    SDL_Window* window = SDL_CreateWindow("2D-Engine",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          windowSizeX, windowSizeY, SDL_WINDOW_SHOWN |
                                          SDL_WINDOW_OPENGL);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED);
    
    if (renderer == nullptr) {
        cerr << "SDL2 Renderer couldn't be created. Error: "
                  << SDL_GetError()
                  << endl;
        exit(1);
    }

    // Create a OpenGL context on SDL2
    this->gl_context = SDL_GL_CreateContext(window);

    // 1 -> vsync : 0 -> NO vsync 
    SDL_GL_SetSwapInterval(0);
    
    // Load GL extensions using glad
    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        cerr << "Failed to initialize the OpenGL context." << endl;
        exit(1);
    }

    // Loaded OpenGL successfully.
    cout << "OpenGL version loaded: " << GLVersion.major << "."
              << GLVersion.minor << endl;

    //sets clear color
    glClearColor(0, 0, 0, 1.0f);

    //line smoothing
    glEnable( GL_LINE_SMOOTH );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //blur and AA
    //glEnable(GL_MULTISAMPLE);

    return make_tuple(window,renderer);
}
void GameManager::internalInit(Config* cfg){

    //stuff used in fixFramerate (spf is used for internal timestep)
    this->tickNumber=0;
    this->fps = cfg->fps;
    this->spf= cfg->spf;
    this->mspf = cfg->mspf;
    this->frameTime = 0;

    this->running = true;
    tuple<SDL_Window*,SDL_Renderer*> winRend = SDL_Visuals_Boilerplate(cfg);
    
    //initalizes screen
    this->screen = Screen();
    double aspectRatio = ((double)windowSizeX) / ((double)windowSizeY);
    this->screen.init(get<0>(winRend),get<1>(winRend),aspectRatio);
    
    //keys
    this->keys = SDL_GetKeyboardState(NULL);

    //event is initalized in header file
}