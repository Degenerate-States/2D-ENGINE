#ifndef enemyManager_h
#define enemyManager_h
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
    public:
        int ID;
        RigidBody rb;
        Polygon poly;

        void init(Assets* assets,RigidBody* plrRB, Stats* stats);
        void spawn(complex<double> pos, complex<double> vel = 0);
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

        void spawnSwarmer(complex<double> pos);

        void update(Screen* screen, double dt);
        void render(Screen* screen);



};
#endif