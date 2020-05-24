#pragma once
#include "components.h"
#include "riggedPoly.h"
#include "config.h"
#include "assets.h"


class Box{
    public:
        RigidBody rb;
        Polygon poly;

        void init(Assets* assets);
        void update(Screen* screen, double dt);
        void render(Screen* screen);

        //callbacks
        void onCollision(int damage, complex<double> direction);
        int getDamage();
};

class RiggedTest{
    public:
        RiggedPoly rp;

        void init(Assets* assets);
        void update(double dt);
        void render(Screen* screen);

        //callbacks
        void onCollision(int damage, complex<double> direction);
        int getDamage();
};

class SnakeTest{
    public: 
        Snake snake;

        void init(Assets* assets,double spf);
        void update(double dt);
        void render(Screen* screen);

        //callbacks
        void onCollision(int damage, complex<double> direction);
        int getDamage();
};

class SkeletonTest{
    public: 
        Skeleton skele;

        void init(Assets* assets);
        void update(double dt);
        void render(Screen* screen);

        //callbacks
        void onCollision(int damage, complex<double> direction);
        int getDamage();
};