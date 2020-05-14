#include "game.h"
#include "engine.h"
Game* game = new Game();

//TODO replace box and rigged box when testing is done
void checkAllCollision(ProjectileManager* projman, EnemyManager* enemyMan, Player* plr,Box* box, RiggedTest* riggedBox){
    //iterates through projectiles
    Point* pntPointer;
    for(int i = 0; i <projman->totalColliders; i++){
        pntPointer = projman->getNextCollider();
        //enemy projectile
        for(int j = 0; j<enemyMan->totalColliders; j++){
            pointPolyCollision(enemyMan->getNextCollider(),pntPointer,pntPointer->collisType);
        }
        //plr projectile
        pointPolyCollision(&plr->poly,pntPointer,pntPointer->collisType);

        //box projectile
        pointPolyCollision(&box->poly,pntPointer,pntPointer->collisType);

        //rigged box
        pointPolyCollision(&riggedBox->rp.poly,pntPointer,pntPointer->collisType);
    }

    //iterates through enemies
    Polygon* enemyPointer;
    for(int i = 0; i < enemyMan->totalColliders; i++){
        enemyPointer = enemyMan -> getNextCollider();

        //plr enemy
        polyPolyCollision(&plr->poly,enemyPointer);

        //box enemy
        polyPolyCollision(&box->poly,enemyPointer);

        //rigged box enemy
        polyPolyCollision(&riggedBox->rp.poly,enemyPointer);

        //enemy enemy (-1 is so its not checking collison with itself)
        for(int i = 0; i < enemyMan->totalColliders-1; i++){
            polyPolyCollision(enemyPointer, enemyMan->getNextCollider());
        }
        //advances to next collider
        enemyMan->skipCollider();
    }

    //unmanaged stuff
    //plr box
    polyPolyCollision(&plr->poly,&box->poly);

    //plr rigged Box
    polyPolyCollision(&plr->poly,&riggedBox->rp.poly);

    //box  rigged box
    polyPolyCollision(&box->poly,&riggedBox->rp.poly);
}

void Start(Screen* screen, Assets* assets, Stats* stats) {
    // initalize game objects
    game->projMan.init(assets,stats);
    game->plr.init(assets,&game->projMan, stats);
    game->enemyMan.init(assets,&game->projMan,&game->plr.rb,stats);
    game->box.init(assets);
    game->rigTest.init(assets);
    game->screen = screen;

    game->nonEnemyPolys.push_back(&game->plr.poly);
    game->nonEnemyPolys.push_back(&game->box.poly);
    game->nonEnemyPolys.push_back(&game->rigTest.rp.poly);
}

void End() {
    // memory cleanup
}

void PreUpdate(double dt) {
    //this->screen.rb.rot = this->plr.rb.rot;
}

void Update(double dt) {
    game->plr.update(game->screen,dt);
    game->box.update(game->screen,dt);
    game->screen->update(dt);
    game->projMan.update(dt);
    game->enemyMan.update(game->screen,dt);
    game->rigTest.update(dt);
}

void PostUpdate(double dt) {
    checkAllCollision(&game->projMan, &game->enemyMan, &game->plr, &game->box, &game->rigTest);
    game->plr.setScreenPos(game->screen,dt);
}

void Render(double dt) {
    //render everything below
    game->plr.render(game->screen);
    game->box.render(game->screen);
    game->projMan.render(game->screen,dt);
    game->enemyMan.render(game->screen);
    game->rigTest.render(game->screen);
}

void Events(SDL_Event* event, Screen* screen,double dt) {
    game->plr.events(event,screen,dt);
}

void Keys(const Uint8* keys,Screen* screen,double dt){
    game->screen->keys(keys,dt);
    game->plr.keys(keys,screen,dt);
}