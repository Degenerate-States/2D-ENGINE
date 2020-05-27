# POLYGUN
2D geometry game and engine for x64 Windows 10

## Setup
Clone

    git clone --recursive git@github.com:Degenerate-States/POLYGUN.git

Inject the vc build environment:

    scripts\shell

Compile

    nmake 

Run

    nmake run 

## Demo Controls

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


## Colliders and Callbacks
there are 2 base colliders in the game:
1. Polygon
2. Point

other higher level components use implement Polygon such as RiggedPolygon, Snake, Skeleton, and hence use the
same collision as Polygon

both polygon and point have a RigidBody component which contains the flag active, which can be used to enable or
disable collision, however they also require an onCollsion and getDamage method to have collsion enabled.
these callbacks are setup as follows
```c++
this->poly.setCallBacks(
    bind(&YourObj::onCollision,this,_1,_2),
    bind(&YourObj::getDamage,this)
);
```
where YourObj has the following methods
```c++
void YourObj::onCollision(int damage, complex<double> direction){
    // on collision behavior
}
int YourObj::getDamage(){
    int damage = 0//however much damage you want to do to the other object in the collision
    return damage;
}
```
when a collision occurs between 2 colliders the damage of each collider is fed into colliders onCollision method.
the direction complex number depends on the type of collision, it is usually the direction to the other collider
except in point hit poly collision, where it is the direction of the point before collision (useful for riccochet).


## Collision Checking
Collision is checked after update function. the engine supports 3 types of collision
1. #### Point Hit Poly
    This type checks if a point will hit a polygon within the next tick, it is fairly expensive compuationally

2. #### Point In Poly
    This type checks if a point is currently inside a polygon

3. #### Poly Poly
    This type checks if any points of each polygon are inside the other

before any of these ways of checking collision are done, the engine does a bunch of prechecks:

    1. it checks if both objects are active (active attribute of rigid body)
    2. checks if both objects have a collsion callback implemented (onCollsion method)
    3. checks if both objects have different collision IDs (way of selectivly disabling collision between objects)
    4. checks if objects are in eachothers circle of influence (is it possible for them to collide)

if all of these pass, more intensive collision checking is done


## Adding Assets
asset storage and addition is done by assets.h and its respective class, polygon points must be defined 
counter-clockwise around the center of the asset (this needed for having properly oriented normals for collision). 
assets are loaded by asset loader on polygon, these can be created using asset-maker and simply pasted as a memeber 
in assets.cpp as done with the player asset.

rigged polygon, snake, and skeleton also require a vector of jointInfo structs to be passed to them. each struct 
corrisponds to a joint which controls the polygon. jointInfo holds a list of vertex indices which are controlled
by the joint, the posistion of the joint relative to the asset, and an optional visual polygon, example:
```c++
this->hostJoints = 
{ jointInfo( {14, 15, 16, 17, 18, 19, 20, 21, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, { 0.13333, 0.0 }, hostEye), 
    jointInfo( {13, 11}, { -0.06667, 0.0 }), 
    jointInfo( {12}, {-0.5, 0.0}) };
```
so the 0th joint controls all vertices in the polygon except 11 12 and 13, it is posistioned at 
(0.13333, 0.0) relative to the asset, and it has a visual polygon hostEye which it moves along with it.

finally skeleton requires additional information about how the joints are linked togeather, this comes in the form
of a vector of linkInfo objects, each containing what the respective joint is connected to, along with how rigid 
those connects are, example:
```c++
this->hostLinks = { linkInfo({1},{10}), linkInfo({2},{5}), linkInfo({},{}) };
```
so joint 0 is connected to the 1st joint with a rigidity of 10, the 1st joint connects to the second with a 
rigidity of 5, and the 2nd joint is connected to nothing (the placeholder is still important)

the linking system can get more complicated, with branching as shown below:
```c++
    this->skeleTestLinks = { linkInfo({1,4,7},{10,10,10}), 
                            linkInfo({2},{7}), linkInfo({3},{5}),linkInfo({},{}),
                            linkInfo({5},{7}), linkInfo({6},{5}),linkInfo({},{}),
                            linkInfo({8},{7}), linkInfo({9},{5}),linkInfo({},{}) };
```
here the 0th joint connects to joints 1,4,7, an important note is that each branch has its own rigidity so
the rigidity vector should be the same length as the index vector. from here joints 1,4,7 are connected to 
joints 2,5,8 with rigidity 7. 2,5,8 are then connected to 3,6,9 whith rigidity 5. and 3,6,9 are connected
to nothing, again the placeholders are required.

FPS info is stored int the Config object in config.h and other initialization information required for the game to run is stored inside the stats object also in config.h

These "asset objects" are initialized by the engine and are passed to the game in Start:
```c++
void Start(Screen* screen, Assets* assets, Stats* stats);
```
