# POLYGUN
2D geometry game and engine in SDL2/GLAD

### Demo Controls

    WASD: movement
    left-click: shoot
    UP/Down: camera zoom
    1-7: guns

## Engine API
```c++
// Called once
void Start(Screen* screen, Assets* assets, Stats* stats);
void End();

// Called per frame
void PreUpdate(double dt);
void Update(double dt);
void PostUpdate(double dt);

void Render(double dt);

void Events(SDL_Event* event, Screen* screen,double dt);
void Keys(const Uint8* keys,Screen* screen,double dt);
```
Components:
game object hold components from components.h according to what functionality they need. Each component may have methods which need to be called once per tick ie update,render. These should be called in a respective update/render api callback function. These in turn are called by Engine during that part of the tick cycle.
```c++
// ie) player has compoent RigidBody
// game.cpp
void Update(double dt) {
    player.update()
}

// in player.cpp
void Player::update(Screen* screen,double dt){
    //other stuff
    this->rigidBody.update()
    //other stuff
}

//similar is true for render and soforth.
```
### Controls and Events
Any game object which is influenced by events must have a .events() method which takes in SDL_Event* event, it should also be placed in the Events api callback.
```c++
void Events(SDL_Event* event, Screen* screen,double dt);
```
Likewise for the state of the keyboard, if your object is influenced by which keys are currently pressed, it must have a .keys() method which takes in const Uint8* keys. it should also be placed in the Keys callback
```c++
void Keys(const Uint8* keys,Screen* screen,double dt);
```

### collision notes
Collision is checked after update function (but before current asset and next asset are swapped), if collision is detected bullets will be moved back to their previous posistion, have  their dynamics altered to riccochet and such, then moved


## Adding Assets
asset storage and addition is done by assets.h and its respective class, polygon points must be defined clockwise
around the center of the asset (this needed for having properly oriented normals for collision). assets are loaded 
by asset loader on polygon, these can be created using point-input-tool and simply pasted as a memeber in assets.cpp as done with the player asset.

FPS info is stored int the Config object in config.h and other initialization information required for the game to run is stored inside the stats object also in config.h

These "asset objects" are initialized by the engine and arre passed to the game in Start:
```c++
void Start(Screen* screen, Assets* assets, Stats* stats);
```
