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
        
        Screen screen;

        Player plr;
        Box box;
        RiggedTest rigTest;
        ProjectileManager projMan;

};

#endif