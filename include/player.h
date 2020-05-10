#ifndef player_h
#define player_h
#include "components.h"
#include "projectileManager.h"
#include "assets.h"
#include "gun.h"
#include "SDL.h"
#include <complex>
#include <vector>

class Flame{
    private:
        RigidBody* plrRb;
        Polygon poly;
        //holds reference to projectile manager so it can make sparks
        ProjectileManager* projMan;
    
    public:
        RigidBody rb;
        void init(RigidBody* plrRb, Assets* assets,ProjectileManager* projMan);
        void update(Screen* screen, double dt);
        void render(Screen* screen);
};

class Player{
    private: 
        // current screen posisiton relative to player
        complex<double> relScreenPos;

        // how quickly the screen snaps to direction of thrusting 
        double screenVel;
        // how far the screen points in the direction of thrusting
        double screenOffset;
    public:
        int ID;
        RigidBody rb;
        Polygon poly;
        Gun gun;
        Flame flame;

        double topSpeed;
        double acceleration;
        
        void init(Assets* assets,ProjectileManager* projMan, Stats* stats);
        void update(Screen* screen,double dt);
        void render(Screen* screen);

        //event hanlding
        void events(SDL_Event* event,Screen* screen,double dt);
        //current keys down
        void keys(const Uint8* keys,Screen* screen,double dt);

        //screen controling
        void setScreenPos(Screen* screen, double dt);
};

#endif