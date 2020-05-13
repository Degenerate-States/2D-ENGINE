#ifndef game_h
#define game_h
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



class Game{
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

        Player plr;
        Box box;
        RiggedTest rigTest;
        ProjectileManager projMan;

};

#endif