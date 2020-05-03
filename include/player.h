#ifndef player_h
#define player_h
#include "components.h"
#include "assets.h"
#include "SDL.h"
#include <complex>
#include <vector>

class Player{
    public:
        RigidBody rb;
        Polygon poly;
        
        void init(Assets* assets);
        void update(double dt);
        void render(Screen* screen);

        //event hanlding
        void events(SDL_Event* event,double dt);
        //current keys down
        void keys(const Uint8* keys,double dt);
};

#endif