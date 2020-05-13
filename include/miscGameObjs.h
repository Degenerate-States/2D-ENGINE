#pragma once
#include "components.h"
#include "riggedPoly.h"
#include "config.h"
#include "assets.h"


class Box{
    public:
        RigidBody rb;
        Polygon poly;
        
        int ID;

        void init(Assets* assets);
        void update(Screen* screen, double dt);
        void render(Screen* screen);
};

class RiggedTest{
    public:
        RiggedPoly rp;

        int ID;

        void init(Assets* assets);
        void update(double dt);
        void render(Screen* screen);
};