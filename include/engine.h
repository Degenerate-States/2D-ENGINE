#ifndef engine_h
#define engine_h
#include "config.h"
#include "assets.h"
#include "SDL.h"
#include <complex>
#include "components.h"
#include "riggedPoly.h"
#include "projectileManager.h"
#include "player.h"
#include "miscGameObjs.h"
#include <tuple>

using namespace std;

extern void Start(Screen screen);
extern void End();

extern void PreUpdate(double dt);
extern void Update(double dt);
extern void PostUpdate(double dt);

extern void Render();

extern void Events(SDL_Event* event, Screen* screen,double dt);
extern void Keys(const Uint8* keys,Screen* screen,double dt);

class Engine{
    public:
        int fps;
        //seconds per frame, used as internal time step dt
        double spf;
        int tickNumber;
        bool running;
        
        ProjectileManager projMan;

        SDL_Event event;
        SDL_GLContext gl_context;
        Screen screen;
        // holds state of current keys
        const Uint8* keys;

        Player plr;
        Box box;
        RiggedTest rigTest;

        
        void init(Config* cfg, Assets* assets,Stats* stats);

        void events(double dt);
        void preUpdateInteractions(double dt);
        void update(double dt);
        void postUpdateInteractions(double dt);
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

        //initalizes all internally used values
        void internalInit(Config*cfg);
        //used in init,returns window and renderer also assigns gl_context
        tuple<SDL_Window*,SDL_Renderer*> SDL_Visuals_Boilerplate(Config* cfg);

        void Check_Quit();

        

};

#endif