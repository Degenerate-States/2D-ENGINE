#ifndef gameManager_h
#define gameManager_h
#include "config.h"
#include "assets.h"
#include "SDL.h"
#include <complex>
#include "components.h"
#include "bulletManager.h"
#include "player.h"
#include "miscGameObjs.h"
#include <tuple>

using namespace std;

class GameManager{
    public:
        int fps;
        //seconds per frame, used as internal time step dt
        double spf;
        int tickNumber;
        bool running;
        
        BulletManager bulletMan;

        SDL_Event event;
        SDL_GLContext gl_context;
        Screen screen;
        // holds state of current keys
        const Uint8* keys;

        Player plr;
        Box box;



        void init(Config* cfg, Assets* assets);

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

        //used in init,returns window and renderer also assigns gl_context
        tuple<SDL_Window*,SDL_Renderer*> SDL_Visuals_Boilerplate(Config* cfg);

};

#endif