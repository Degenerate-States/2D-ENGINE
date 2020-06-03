#include "engine.h"

//#define SOUND true

class Engine{
public:
    ~Engine();
    static Engine* instance();
    //disable copying
	Engine(const Engine&);
    Engine& operator= (const Engine&);



    int fps;
    //seconds per frame, used as internal time step dt
    double spf;
    int tickNumber;
    bool running;
    
    SDL_Event event;
    SDL_GLContext gl_context;
    Screen screen;
    // holds state of current keys
    const Uint8* keys;

    void init(Config* cfg, Assets* assets,Stats* stats);
    void events(double dt);
    void render();
    //also increases ticknumber
    void fixFramerate();
    void gameLoop();
    void clean();

private:
    //both used by fixFramerate
    Uint32 frameTime;
    //miliseconds per frame
    int mspf;

    //used in init,returns window and renderer also assigns gl_context
    tuple<SDL_Window*,SDL_Renderer*> SDL_Visuals_Boilerplate(Config* cfg);

    void Check_Quit();

    Engine() { }

};


int main(int argc, char **argv) {
    Assets* assets = new Assets();
    Config* cfg = new Config();
    Stats* stats = new Stats();
    assets->init();
    cfg->init();
    stats->init();

    Engine::instance()->init(cfg,assets,stats);
    EventDispatcher::instance()->init();

    Start(&Engine::instance()->screen, assets, stats,cfg->spf);

    free(assets);
    free(cfg);
    free(stats);

    #if SOUND
        Audio::instance()->init();
        //TODO: add a valid-formatted song for testing
        //audio->playSound(MUS_PATH, VOLUME);
        Audio::instance()->unpauseAudio();
    #endif

    Engine::instance()->gameLoop();

    #if SOUND
        Audio::instance()->clean();
    #endif

    End();

    //engine.clean();
    return 0;
}




Engine* Engine::instance() {
    static Engine* a = new Engine();
    return a;
}

//************************//
//**  Engine Methods    **//
//************************//
void Engine::init(Config* cfg,Assets* assets,Stats* stats){
    //stuff used in fixFramerate (spf is used for internal timestep)
    this->tickNumber=0;
    this->fps = cfg->fps;
    this->spf= cfg->spf;
    this->mspf = cfg->mspf;
    this->frameTime = 0;

    this->running = true;
    tuple<SDL_Window*,SDL_Renderer*> winRend = SDL_Visuals_Boilerplate(cfg);
    
    //initalizes screen
    double aspectRatio = ((double)windowSizeX) / ((double)windowSizeY);
    this->screen.init(get<0>(winRend),get<1>(winRend),aspectRatio);
    
    //keys
    this->keys = SDL_GetKeyboardState(NULL);

    //event is initalized in header file
    this->screen.zoom = stats->defaultZoom;

}

void Engine::events(double dt){
    //Also updates key state array
    while (SDL_PollEvent(&(this->event)))
    {
        this->Check_Quit();

        //game object events
        Events(&(this->event),&this->screen,dt);
    }

    if (this->keys[SDL_SCANCODE_P]) {
        Audio::instance()->pauseAudio();
    }
    if (this->keys[SDL_SCANCODE_U]) {
        Audio::instance()->unpauseAudio();
    }

    // keypresses
    Keys(this->keys,&this->screen,dt);
}

void Engine::render(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Render(this->spf);

    SDL_GL_SwapWindow(this->screen.window);
}



void Engine::gameLoop(){
    // Loop condition
    while (this->running) {
        this->events(this->spf);
        PreUpdate(this->spf);
        Update(this->spf);
        PostUpdate(this->spf);
        this->render();
        this->fixFramerate();
    }
}

void Engine::clean(){
    // Destroy everything to not leak memory.
    SDL_GL_DeleteContext(this->gl_context);
    SDL_DestroyRenderer(this->screen.renderer);
    SDL_DestroyWindow(this->screen.window);

    SDL_Quit();
}

void Engine::Check_Quit(){
    //events
    switch(this->event.type) {
        case SDL_QUIT:
            running = false;
            exit(0);
            break;

        case SDL_KEYDOWN:
            switch(this->event.key.keysym.sym) {
            case SDLK_ESCAPE:
                running = false;
                exit(0);
                break;
            }
    }
}

void Engine::fixFramerate(){
    this->tickNumber+=1;
    double frameDuration = SDL_GetTicks() -this->frameTime;

    if (this->tickNumber%300 == 0){
        double currentfps = 1000.0 / frameDuration;
        cout <<"fps: "<< currentfps <<  endl;
    }

    if (this->mspf > frameDuration){
        SDL_Delay(this->mspf - frameDuration);
    }
    this->frameTime = SDL_GetTicks();
}

// TODO: refactor
tuple<SDL_Window*,SDL_Renderer*> Engine::SDL_Visuals_Boilerplate(Config* cfg){
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
