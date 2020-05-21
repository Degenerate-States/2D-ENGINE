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

        int startHealth;
        int contactDamage;

        void animate();
    public:
        int health;

        RigidBody rb;
        Polygon poly;

        void init(Assets* assets,RigidBody* plrRB, Stats* stats);
        void spawn(complex<double> pos, complex<double> vel);
        void die();

        void update(double dt);
        void render(Screen* screen);

        //callbacks
        void onCollision(int Damage, complex<double> direction);
        int getDamage();
};

class Host{
    private:
        RigidBody* plrRB;

        //stats
        double speed;

        int startHealth;
        int contactDamage;

        void animate();

    public:
        int health;

        RigidBody rb;
        Polygon poly;

        void init(Assets* assets,RigidBody* plrRB, Stats* stats);
        void spawn(complex<double> pos, complex<double> vel);
        void die();

        void update(double dt);
        void render(Screen* screen);

        //callbacks
        void onCollision(int Damage, complex<double> direction);
        int getDamage();
};





class EnemyManager{
    private:

        int oldestSwarmerIndex;
        Swarmer* swarmers[swarmerPoolSize];

        //collider iteration
        int nextCollider;
    public:
        

        void init(Assets* assets, ProjectileManager* projMan,RigidBody* plrRB, Stats* stats);

        void spawnSwarmer(complex<double> pos, complex<double> vel = 0, double velVarience = 0);

        void update(Screen* screen, double dt);
        void render(Screen* screen);

        //collider iteration
        int totalColliders;
        Polygon* getNextCollider();
        void EnemyManager::skipCollider();
};