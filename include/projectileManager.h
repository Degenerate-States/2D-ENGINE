#ifndef projectileManager_h
#define projectileManager_h
#include "components.h"
#include "config.h"
#include <complex>
#include <vector>
using namespace std;

enum projectileType{ bullet, spark };

class Bullet{
    public:
        projectileType type;

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
        void activate(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, 
                        int shooterID, complex<double> pos, complex<double> vel);
        void update(double dt);
        void render(Screen* screen,double dt);
        //returns riccochet direction
        complex<double> riccochet(complex<double> normal);
};

class Spark{
    public:
        projectileType type;
        RigidBody rb;
        Point pnt;
        Trail trail;
        bool active;
        void init(Stats* stats);

        void activate(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, 
                    complex<double> pos, complex<double> vel);
        void update(double dt);
        void render(Screen* screen,double dt);
};

class ProjectileManager{
    private:
        int oldestBulletIndex;
        Bullet* bullets[bulletPoolSize];

        int oldestSparkIndex;
        Spark* sparks[sparkPoolSize];
        
        //divides riccochet vel by this to get num sparks
        double riccoSparkSpawnDamping;
        //divides riccochet vel by this to get spark vel
        double riccoSparkVelDamping;
        double sparkVelVarience;

        void onCollsion(int bulletIndex,int hitID,complex<double> collisionPoint, complex<double> collisionNormal);
    
    public:
        void init(Stats* stats);
        void fireBullet(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, int shooterID, 
                        complex<double> pos, complex<double> vel, double velVarience);
        //vel varience is maximum deviation from vel (think radius of circle around vel)
        void fireSpark(tuple<int,int,int> headColor,tuple<int,int,int> tailColor,
                        complex<double> pos, complex<double> vel, double velVarience);

        void update(double dt);
        // checks polygon against all bullets
        void checkCollisionPoly(int ID,RigidBody* rb,Polygon* poly,double dt);
        void render(Screen* screen,double dt);
};

#endif