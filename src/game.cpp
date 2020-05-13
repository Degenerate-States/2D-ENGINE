#include "game.h"
#include "engine.h"

Game* game = new Game();

void Start(Screen* screen, Assets* assets, Stats* stats) {
    // initalize game objects
    game->projMan.init(assets,stats);
    game->plr.init(assets,&game->projMan, stats);
    game->box.init(assets);
    game->rigTest.init(assets);
    game->screen = screen;
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
    game->rigTest.update(dt);
}
void PostUpdate(double dt) {
    game->projMan.checkCollisionPoly(&game->box.poly,game->box.ID,dt);
    game->projMan.checkCollisionPoly(&game->plr.poly,game->plr.ID,dt);
    game->projMan.checkCollisionPoly(&game->rigTest.rp.poly,game->rigTest.ID,dt);
    game->plr.setScreenPos(game->screen,dt);
}


void Render() {
    //render everything below
    game->plr.render(game->screen);
    game->box.render(game->screen);
    game->projMan.render(game->screen,game->spf);
    game->rigTest.render(game->screen);
}

void Events(SDL_Event* event, Screen* screen,double dt) {
    game->plr.events(event,screen,dt);
}
void Keys(const Uint8* keys,Screen* screen,double dt){
    game->screen->keys(keys,dt);
    game->plr.keys(keys,screen,dt);
}
