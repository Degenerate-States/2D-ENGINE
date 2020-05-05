#include "gameManager.h"
#include "components.h"
#include "config.h"
#include "assets.h"
#include <SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <complex>
#include <tuple>
using namespace std;




//************************//
//**GameManager Methods **//
//************************//
void GameManager::init(Config* cfg,Assets* assets){
    //stuff used in fixFramerate (spf is used for internal timestep)
    this->tickNumber=0;
    this->fps = cfg->fps;
    this->spf= cfg->spf;
    this->mspf = cfg->mspf;
    this->frameTime = 0;

    this->bulletMan.init(cfg);

    this->plr.init(assets,&this->bulletMan);
    this->running = true;

    tuple<SDL_Window*,SDL_Renderer*> winRend = SDL_Visuals_Boilerplate(cfg);
    
    //initalizes screen
    this->screen = Screen();
    double aspectRatio = ((double)windowSizeX) / ((double)windowSizeY);
    this->screen.init(get<0>(winRend),get<1>(winRend),aspectRatio);
    
    //keys
    this->keys = SDL_GetKeyboardState(NULL);

    //event is initalized in header file
}


void GameManager::events(double dt){
    //Also updates key state array
    while (SDL_PollEvent(&(this->event)))
    {
        //events
    switch(this->event.type) {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_KEYDOWN:
            switch(this->event.key.keysym.sym) {
            case SDLK_ESCAPE:
                running = false;
                break;
        

            }
        }
    //game object events
    this->plr.events(&(this->event),&this->screen,dt);
    }

    // keypresses
    this->screen.keys(this->keys,dt);
    this->plr.keys(this->keys,dt);

}
void GameManager::preUpdateInteractions(double dt){
    //this->screen.rb.rot = this->plr.rb.rot;

}
void GameManager::update(double dt){
    this->plr.update(&this->screen,dt);
    this->screen.update(dt);
    this->bulletMan.update(dt);
}
void GameManager::postUpdateInteractions(double dt){
    //this->screen.rb.pos = this->plr.rb.pos;
}
void GameManager::render(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //render everything below
    this->plr.render(&(this->screen));
    this->bulletMan.render(&this->screen);

    SDL_GL_SwapWindow(this->screen.window);
}
void GameManager::fixFramerate(){
    this->tickNumber+=1;
    double frameDuration = SDL_GetTicks() -this->frameTime;

    double currentfps = 1000.0 / frameDuration;

    if (this->tickNumber%300 == 0){
        std::cout <<"fps: "<< currentfps <<  std::endl;
    }

    if (this->mspf > frameDuration){
        SDL_Delay(this->mspf - frameDuration);
    }
    this->frameTime = SDL_GetTicks();
}


void GameManager::gameLoop(){
    // Loop condition
    while (this->running) {
        this->events(this->spf);
        this->preUpdateInteractions(this->spf);
        this->update(this->spf);
        this->postUpdateInteractions(this->spf);
        this->render();
        this->fixFramerate();
    }
}


void GameManager::clean(){
    // Destroy everything to not leak memory.
    SDL_GL_DeleteContext(this->gl_context);
    SDL_DestroyRenderer(this->screen.renderer);
    SDL_DestroyWindow(this->screen.window);

    SDL_Quit();
}



