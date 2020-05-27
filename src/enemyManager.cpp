#include "enemyManager.h"

// Swarmer
void Swarmer::init(Assets* assets,RigidBody* plrRB, Stats* stats){
    this->rb.init(1.,0,0,0.0);

    this->topSpeed = stats->swarmerTopSpeed;
    this->acceleration = stats->swarmerAcceleration;
    this->drag = stats->swarmerDrag;

    this->poly.init(&assets->swarmerAsset,&this->rb,red,assets->getID());
    this->plrRB = plrRB;
    this->rb.active = false;

    this->startHealth = stats->swarmerHealth;
    //setsup poly collision callbacks
    this->poly.setCallBacks(
        bind(&Swarmer::onCollision,this,_1,_2),
        bind(&Swarmer::getDamage,this)
    );
}

void Swarmer::spawn(complex<double> pos, complex<double> vel){
    this->rb.active = true;
    this->rb.pos = pos;
    this->rb.vel = vel;

    this->health = this->startHealth;
}
void Swarmer::die(){
    this->rb.active = false;
    cout<<"Debug: Swarmer died"<<endl;
}
void Swarmer::update(double dt){
    // accelerates in direction of player
    complex<double> direction = this->plrRB->pos - this->rb.pos;
    direction *= this->acceleration/abs(direction);
    this->rb.applyForce(real(direction),imag(direction),dt);

    //points in direction of motion
    this->rb.setRot(arg(this->rb.vel));

    //clamps velocity
    if (abs(this->rb.vel) > this->topSpeed){
        this->rb.vel *= this->topSpeed/abs(this->rb.vel);
    }
    double speed = abs(this->rb.vel);
    this->rb.vel -= this->drag*dt*this->rb.vel;

    //animates
    this->animate();

    this->rb.update(dt);
    this->poly.update();

    if (this->health <= 0){
        this->die();
    }
}

void Swarmer::render(Screen* screen){
    this->poly.render(screen);
}
    //Callbacks
void Swarmer::onCollision(int damage, complex<double> direction){
    this->health-=damage;
    #if COLLISION_PRINT
    cout<<"Debug: swarmer hit...   Health:"<<" "<<this->health<<endl;
    #endif
}
int Swarmer::getDamage(){
    return this->contactDamage;
}
    //Private
void Swarmer::animate(){
    //chomp
    this->poly.vertexOffsets[0] = {0,0.01*cos(0.03*SDL_GetTicks())};
    this->poly.vertexOffsets[4] = {0,-0.01*cos(0.03*SDL_GetTicks())};
}






//EnemyManager
void EnemyManager::init(Assets* assets, ProjectileManager* projMan,RigidBody* plrRB, Stats* stats){
    //initalize swarmers
    this->oldestSwarmerIndex = 0;
    for(int i = 0; i < swarmerPoolSize; i++){
        this->swarmers[i] = new Swarmer();
        this->swarmers[i]->init(assets,plrRB,stats);
    }

    //TODO remove tester swarmer spawn below
    this->spawnSwarmer({1,1});

    // for iterating through pools
    this->totalColliders = swarmerPoolSize;
    this->nextCollider = 0;
}

void EnemyManager::spawnSwarmer(complex<double> pos, complex<double> vel, double velVarience){
    vel += randComplex(velVarience);

    this->swarmers[this->oldestSwarmerIndex]->spawn(pos, vel);
    // change which bullet is considered the oldest
    this->oldestSwarmerIndex+=1;
    this->oldestSwarmerIndex%=swarmerPoolSize;
}

void EnemyManager::update(Screen* screen, double dt){
    //swarmers
    for(int i = 0; i < swarmerPoolSize; i++){
        if(this->swarmers[i]->rb.active){
            this->swarmers[i]->update(dt);
        }
    }
}
void EnemyManager::render(Screen* screen){
    for(int i = 0; i < swarmerPoolSize; i++){
        if(this->swarmers[i]->rb.active){
            this->swarmers[i]->render(screen);
        }
    }
}

Polygon* EnemyManager::getNextCollider(){
    Polygon* polyPointer;
    if(this->nextCollider<bulletPoolSize){
        polyPointer = &this->swarmers[this->nextCollider]->poly;
    }
    //additional pools here
    else{
        cout<<"Debug: getNextCollider, Collider Index out of Range"<<endl;
    }
    this->nextCollider++;
    this->nextCollider%=this->totalColliders;

    return polyPointer;
}
void EnemyManager::skipCollider(){
    this->nextCollider++;
    this->nextCollider%=this->totalColliders;
}