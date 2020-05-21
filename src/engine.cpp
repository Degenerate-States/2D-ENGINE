#include "engine.h"

class Engine{
    public:
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

};

// TODO: move audio into a class
static Uint8 *audio_pos; // global pointer to the audio buffer to be played
static Uint32 audio_len; // remaining length of the sample we have to play

#define MUS_PATH "assets/music/metal.wav"
#define VOLUME SDL_MIX_MAXVOLUME/3

static Uint32 wav_length; // length of our sample
static Uint8 *wav_buffer; // buffer containing our audio file
static SDL_AudioSpec wav_spec; // the specs of our piece of music

// sdl audio callback
void my_audio_callback(void *userdata, Uint8 *stream, int len) {
	
	if (audio_len ==0)
		return;
	
	len = ( len > audio_len ? audio_len : len );
	SDL_memset(stream, 0, len);
	SDL_MixAudio(stream, audio_pos, len, VOLUME);// mix from one buffer into another
	
	audio_pos += len;
	audio_len -= len;
}

void init_audio() {
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	    exit(-1);

	if( SDL_LoadWAV(MUS_PATH, &wav_spec, &wav_buffer, &wav_length) == NULL ){
	  cerr << "Couldn't load wave: " << SDL_GetError() << endl;
	  exit(-1);
	}

	wav_spec.callback = my_audio_callback;
	wav_spec.userdata = NULL;
    
	audio_pos = wav_buffer;
	audio_len = wav_length;
	
	if ( SDL_OpenAudio(&wav_spec, NULL) < 0 ){
	  cerr << "Couldn't open audio: " << SDL_GetError() << endl;
	  exit(-1);
	}
	
}

// for audio thread
static int playMusic(void *ptr) {
	SDL_PauseAudio(0);

	while ( audio_len > 0 ) {
		SDL_Delay(10); 
	}

    return 0;
}




int main(int argc, char **argv) {
    Assets* assets = new Assets();
    Config* cfg = new Config();
    Stats* stats = new Stats();
    assets->init();
    cfg->init();
    stats->init();

    Engine engine;
    engine.init(cfg,assets,stats);

    Start(&engine.screen, assets, stats,cfg->spf);

    free(assets);
    free(cfg);
    free(stats);

    // fire audio thread

    init_audio();
    SDL_Thread *audioThread;
    audioThread = SDL_CreateThread(playMusic, "AudioThread", (void *)NULL);
    
	
    engine.gameLoop();

    // TEMP audio cleanup
	SDL_CloseAudio();
	SDL_FreeWAV(wav_buffer);

    End();

    engine.clean();
    return 0;
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
            break;

        case SDL_KEYDOWN:
            switch(this->event.key.keysym.sym) {
            case SDLK_ESCAPE:
                running = false;
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
