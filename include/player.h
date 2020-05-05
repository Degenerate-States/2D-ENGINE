#ifndef player_h
#define player_h
#include "components.h"
#include "bulletManager.h"
#include "assets.h"
#include "SDL.h"
#include <complex>
#include <vector>

class Gun{
    public:
        RigidBody rb;
        Polygon poly;
        //holds reference to bullet manager so it can fire
        BulletManager* bulletMan;
        //holds copy of shooters id
        int ID;

        void init(int ID, Assets* assets,BulletManager* bulletMan);
        void update(Screen* screen,RigidBody*rb, double dt);
        void render(Screen* screen);

        //event hanlding
        void events(SDL_Event* event, Screen* screen ,double dt);
};


class Player{
    public:
        int ID;
        RigidBody rb;
        Polygon poly;
        Gun gun;
        
        void init(Assets* assets,BulletManager* bulletMan);
        void update(Screen* screen,double dt);
        void render(Screen* screen);

        //event hanlding
        void events(SDL_Event* event,Screen* screen,double dt);
        //current keys down
        void keys(const Uint8* keys,double dt);
};

#endif