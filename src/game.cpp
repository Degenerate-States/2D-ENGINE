#include "game.h"
#include "engine.h"
#include <imgui.h>
Game* game = new Game();

//TODO replace box and rigged box when testing is done
void checkAllCollision(ProjectileManager* projman, EnemyManager* enemyMan, vector<Polygon*>* nonEnemyPolys){
    //iterates through projectiles
    Point* pntPointer;
    for(int i = 0; i <projman->totalColliders; i++){
        pntPointer = projman->getNextCollider();
        //enemy / projectile
        for(int j = 0; j<enemyMan->totalColliders; j++){
            pointPolyCollision(enemyMan->getNextCollider(),pntPointer,pntPointer->collisType);
        }
        //non enemy / projectile
        for(long unsigned int j = 0; j<nonEnemyPolys->size(); j++){
            pointPolyCollision((*nonEnemyPolys)[j],pntPointer,pntPointer->collisType);
        }
    }

    //iterates through enemies
    Polygon* enemyPointer;
    for(int i = 0; i < enemyMan->totalColliders; i++){
        enemyPointer = enemyMan -> getNextCollider();

        //non enemy / enemy
        for(long unsigned int j = 0; j<nonEnemyPolys->size(); j++){
            polyPolyCollision((*nonEnemyPolys)[j],enemyPointer);
        }

        //enemy enemy (-1 is so its not checking collison with itself)
        for(int i = 0; i < enemyMan->totalColliders-1; i++){
            polyPolyCollision(enemyPointer, enemyMan->getNextCollider());
        }
        //advances to next collider
        enemyMan->skipCollider();
    }

    //non enemy / non enemy
    for(long unsigned int i = 0; i<nonEnemyPolys->size()-1; i++){
        for(long unsigned int j = i+1; j<nonEnemyPolys->size();j++){
            polyPolyCollision((*nonEnemyPolys)[i],(*nonEnemyPolys)[j]);
        }
    }
}

void Start(Screen* screen, Assets* assets, Stats* stats,double dt) {
    // initalize game objects
    game->projMan.init(assets,stats);
    game->plr.init(assets,&game->projMan, stats);
    game->enemyMan.init(assets,&game->projMan,&game->plr.rb,stats);
    game->box.init(assets);
    game->snakeTest.init(assets,dt);

    game->skeleTest.init(assets , 2 , 1);
    game->skeleTest1.init(assets, 0 , 1);
    game->skeleTest2.init(assets, -2, 1);

    game->screen = screen;

    game->nonEnemyPolys.push_back(&game->plr.poly);
    game->nonEnemyPolys.push_back(&game->box.poly);
    game->nonEnemyPolys.push_back(&game->snakeTest.snake.rp.poly);
    game->nonEnemyPolys.push_back(&game->skeleTest.skele.rp.poly);
    game->nonEnemyPolys.push_back(&game->skeleTest1.skele.rp.poly);
    game->nonEnemyPolys.push_back(&game->skeleTest2.skele.rp.poly);
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
    game->snakeTest.update(dt);
    game->skeleTest.update(dt);
    game->skeleTest1.update(dt);
    game->skeleTest2.update(dt);
}

void PostUpdate(double dt) {
    checkAllCollision(&game->projMan, &game->enemyMan,&game->nonEnemyPolys);
    game->plr.setScreenPos(game->screen,dt);
}

void Render(double dt) {
    //render everything below
    game->plr.render(game->screen);
    game->box.render(game->screen);
    game->projMan.render(game->screen,dt);
    game->enemyMan.render(game->screen);
    game->snakeTest.render(game->screen);
    game->skeleTest.render(game->screen);
    game->skeleTest1.render(game->screen);
    game->skeleTest2.render(game->screen);
    #if RENDER_GUI
        ImGui::SetNextItemOpen(true, ImGuiCond_Once); // default to open
        if (ImGui::TreeNode("Player")) {
            ImGui::Text("Pos: %.2lf, %.2lf", game->plr.rb.pos.real(), game->plr.rb.pos.imag());
            ImGui::Text("Vel: %.2lf, %.2lf", game->plr.rb.vel.real(), game->plr.rb.vel.imag());
            ImGui::Text("Rot: %.2lf", game->plr.rb.rot);
            ImGui::Text("topSpeed: %.2lf", game->plr.topSpeed);
            ImGui::Text("acceleration: %.2lf", game->plr.acceleration);
            ImGui::Text("Health: %d", game->plr.health);
            ImGui::TreePop();
        }
    #endif
}

void Events(SDL_Event* event, Screen* screen,double dt) {
    game->plr.events(event,screen,dt);
}

void Keys(const Uint8* keys,Screen* screen,double dt){
    game->screen->keys(keys,dt);
    game->plr.keys(keys,screen,dt);
}