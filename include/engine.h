#pragma once
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <glad/glad.h>

#include <tuple> // sdl boilerplate
#include <iostream>  // fps prints and 

// config 
#include "config.h"
#include "assets.h"


using namespace std;


// Engine Callback API
// Called once
extern void Start(Screen* screen, Assets* assets, Stats* stats);
extern void End();
// Called per frame
extern void PreUpdate(double dt);
extern void Update(double dt);
extern void PostUpdate(double dt);

extern void Render(double dt);

extern void Events(SDL_Event* event, Screen* screen,double dt);
extern void Keys(const Uint8* keys,Screen* screen,double dt);


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