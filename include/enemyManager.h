#pragma once
#include "components.h"
#include "projectileManager.h"
#include "assets.h"
#include "config.h"


class Swarmer{
    private:
        RigidBody* plrRB;

        //stats
        double topSpeed;
        double acceleration;
        double drag;
    public:
        int ID;
        RigidBody rb;
        Polygon poly;

        void init(Assets* assets,RigidBody* plrRB, Stats* stats);
        void spawn(complex<double> pos, complex<double> vel);
        void die();

        void update(double dt);
        void render(Screen* screen);

};

class EnemyManager{
    private:

        int oldestSwarmerIndex;
        Swarmer* swarmers[swarmerPoolSize];
    public:

        void init(Assets* assets, ProjectileManager* projMan,RigidBody* plrRB, Stats* stats);

        void spawnSwarmer(complex<double> pos, complex<double> vel = 0, double velVarience = 0);

        void update(Screen* screen, double dt);
        void checkCollision(ProjectileManager* projMan,double dt);
        void render(Screen* screen);

};