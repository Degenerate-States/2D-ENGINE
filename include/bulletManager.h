#ifndef bulletManager_h
#define bulletManager_h
#include "components.h"
#include "config.h"
#include <complex>
#include <vector>
using namespace std;

class Bullet{
    public:
        // 0 is all, 1 is all but player, 2 is all but enemies 
        int shooterID;
        complex<double> prevPos;
        RigidBody rb;
        Point pnt;
        Trail trail;
        //bool must be checked to see if you should apply render, collision and update 
        bool active;

        // will also include trail component

        void init(Stats* stats);
        void activate(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, int shooterID,double diameter, 
                        double mass, complex<double> pos, complex<double> vel);
        void update(double dt);
        void render(Screen* screen,double dt);
        void riccochet(complex<double> normal,double dt);
};

class BulletManager{
    private:
        int oldestBulletIndex;
        Bullet* bullets[bulletPoolSize];
        
        
    
    public:
        void init(Stats* stats);
        void fireBullet(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, int shooterID,double diameter, 
                        double mass,complex<double> pos, complex<double> dirVec, double speed);

        // checks polygon against all bullets
        void checkCollisionPoly(int ID,RigidBody* rb,Polygon* poly,double dt);

        void update(double dt);
        void render(Screen* screen,double dt);
};

#endif