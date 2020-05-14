#pragma once
#include "components.h"
#include "config.h"
#include "assets.h"

#include <complex>
#include <vector>
#include <tuple>

using namespace std;

complex<double> randComplex(double maxMag);

class Bullet{
    private:
        double riccoVelDamping;
        RigidBody* homingTarget;

        double homingRate;

        double velDamping;
        double minVel;

        double spawnSpeed;
        double startDiameter;



    public:
        projectileType type;

        RigidBody rb;
        Point pnt;
        Trail trail;
        //bool must be checked to see if you should apply render, collision and update 

        // will also include trail component

        void init(Stats* stats);
        //homing target defaults to null, if target is null no homing is applied, likewise if the target is inactive
        void activate(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, int shooterID, complex<double> pos, 
                        complex<double> vel, RigidBody* homingTarget, double homingRate);
        void update(double dt);
        void render(Screen* screen,double dt);

        //callback fuctions
        void onCollision(int damage, complex<double> collisionNormal);
        int getDamage();

        //used for spark spawning
        function<void(complex<double>, complex<double>)>   collisonSparksCallback;
        


};

class Spark{
    private:
        double velDamping;
        double minVel;

        // values at spawn, decreased overtime
        double spawnSpeed;
        double startDiameter;
    public:
        projectileType type;
        RigidBody rb;
        Point pnt;
        Trail trail;
        void init(Stats* stats);

        void activate(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, 
                    complex<double> pos, complex<double> vel);
        void update(double dt);
        void render(Screen* screen,double dt);
};

class EnergyBall{
    private:
        double vibeFreq;
        double vibeAmp;
        double vibePhase;

        double explosionTotalTime;
        double explosionTimeRemaining;
        double explosionVel;
        double explosionVelVarience;

        RigidBody* homingTarget;
        double homingRate;

        void explode(double dt);
    public:
        projectileType type;
        RigidBody rb;
        Polygon outerPoly;
        Polygon innerPoly;

        //not rendered, used by collision engine
        Point pnt;

        bool exploding;


        void init(Assets* assets,Stats* stats);

        void activate(tuple<int,int,int> innerColor,tuple<int,int,int> outerColor, int shooterID, complex<double> pos, 
                        complex<double> vel, RigidBody* homingTarget, double homingRate);
        void update(double dt);
        void render(Screen* screen);

        //callback fuctions
        void onCollision(int damage, complex<double> direction);
        int getDamage();
};

class ProjectileManager{
    private:
        int oldestBulletIndex;
        Bullet* bullets[bulletPoolSize];

        int oldestSparkIndex;
        Spark* sparks[sparkPoolSize];

        int oldestEngBallIndex;
        EnergyBall* engBalls[engBallPoolSize];
        
        //divides riccochet vel by this to get num sparks
        double riccoSparkSpawnDamping;
        //divides riccochet vel by this to get spark vel
        double riccoSparkVelDamping;
        double riccoSparkVelVarience;


    public:
        void init(Assets* assets, Stats* stats);

        //vel varience is maximum deviation from vel (think radius of circle around vel)
        void fireBullet(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, int shooterID, complex<double> pos, 
                        complex<double> vel, double velVarience, RigidBody* homingTarget = NULL,double homingRate = 0.0);
        void fireSpark(tuple<int,int,int> headColor,tuple<int,int,int> tailColor,
                        complex<double> pos, complex<double> vel, double velVarience);
        //vel varience is maximum deviation from vel (think radius of circle around vel)
        void fireEngBall(tuple<int,int,int> headColor,tuple<int,int,int> tailColor, int shooterID, complex<double> pos, 
                        complex<double> vel, double velVarience, RigidBody* homingTarget = NULL,double homingRate = 0.0);
        
        void update(double dt);
        // checks polygon against all bullets
        void checkCollisionPoly(Polygon* poly);
        void render(Screen* screen,double dt);

        void collisionSparks(complex<double> direction,complex<double> point);
};