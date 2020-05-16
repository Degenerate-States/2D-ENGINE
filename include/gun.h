#pragma once
#include "components.h"
#include "projectileManager.h"
#include "assets.h"


class Gun{
    private:
        int lastFired;

    public:
        projectileType type;
        RigidBody rb;
        Polygon poly;
        //holds reference to bullet manager so it can fire
        ProjectileManager* projMan;
        //holds copy of shooters id
        int ID;

        //gun stats
        bool fullAuto;
        // in miliseconds
        double coolDown;
        double projVel;
        double velVarience;
        int projNum;
        double homingRate;
        double baseDamage;

        //holds reference of shooters rb
        RigidBody* shooterRb;

        void init(RigidBody* shooterRb, Assets* assets,ProjectileManager* projMan, GunStats* stats, int ID);
        void update(Screen* screen, double dt);
        void render(Screen* screen);

        //event hanlding
        void events(SDL_Event* event, Screen* screen ,double dt);
        //target is homing target, defaults to null
        void fire(complex<double> fireDirection,RigidBody* target = NULL);
        //wrapper for loading in gun stats
        void equip(GunStats* stats);
};