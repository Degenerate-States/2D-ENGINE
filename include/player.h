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

        double bulletVel;
        //holds reference of shooters rb
        RigidBody* plrRb;

        void init(RigidBody* plrRb, Assets* assets,BulletManager* bulletMan,int ID, double bulletVel);
        void update(Screen* screen, double dt);
        void render(Screen* screen);

        //event hanlding
        void events(SDL_Event* event, Screen* screen ,double dt);
};

class Flame{
    private:
        RigidBody* plrRb;
        Polygon poly;
        //holds reference to bullet manager so it can make sparks
        BulletManager* bulletMan;
    
    public:
        RigidBody rb;
        void init(RigidBody* plrRb, Assets* assets,BulletManager* bulletMan);
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
        
        void init(Assets* assets,BulletManager* bulletMan, Stats* stats);
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