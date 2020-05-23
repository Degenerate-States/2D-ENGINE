#pragma once
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <glad/glad.h>

#include <tuple> // sdl boilerplate
#include <iostream>  // fps prints and 

// config 
#include "config.h"
#include "assets.h"

#include "sound.h"

using namespace std;


// Engine Callback API
// Called once
extern void Start(Screen* screen, Assets* assets, Stats* stats,double dt);
extern void End();
// Called per frame
extern void PreUpdate(double dt);
extern void Update(double dt);
extern void PostUpdate(double dt);

extern void Render(double dt);

extern void Events(SDL_Event* event, Screen* screen,double dt);
extern void Keys(const Uint8* keys,Screen* screen,double dt);