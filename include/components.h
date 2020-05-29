#pragma once

#include "config.h"

#include <SDL.h>
#include <glad/glad.h>

#include <complex>
#include <cmath>
#include <vector>
#include <tuple>
#include <iostream>
#include <functional>
using namespace std;
using namespace std::placeholders;

#define _USE_MATH_DEFINES

enum collisionType{ pointHitPoly, pointInPoly, polyPoly };

class RigidBody{
    public:
        bool active;
        double mass;
        complex<double> pos;
        complex<double> prevPos;
        complex<double> vel;

        //in radians
        double rot;
        double rotVel;

        //complex used to rotate other complex number by rot though multiplication
        //note multiplying by its complex conj will be the negative of this rotation
        //rotOp is updated in update method, if its going to be used update method must be called
        complex<double> rotOp;
        
        void init(double m, double x, double y, double initRot);

        // used if you dont have complex
        void setPos(double x, double y);
        //displace should be avoided for constant movement as it doesnt depend on dt (hence will vary with fps)
        void displace(double dx, double dy);
        void fixedDisplace(double vx,double vy, double dt);

        void setVel(double vx, double vy);
        void applyForce(double fx, double fy,double dt);

        void setRot(double theta);
        //for single use rotations, (ie suddenly turning somting pi rad to the right)
        void rotate(double dTheta);
        //fixed rotate is for framerate invarient rotations (ie player controls)
        void fixedRotate(double rotVel,double dt);

        void update(double dt);
};


class Screen{
    public:
        //allows for screen rotation and translations in world coordinates
        RigidBody rb;

        SDL_Window* window;
        SDL_Renderer* renderer;

        double aspectRatio;
        double zoom;
        
        void init(SDL_Window* win, SDL_Renderer* rend,double winAspectRatio);
        void update(double dt);
        //methods for converting coordinate systems
        tuple<double,double> worldToScreen(complex<double> val);
        complex<double> screenToWorld(double x, double y);
        complex<double> pixelScreenToWorld(int x, int y);

        //inc or dec by zoomVel*dt 
        void changeZoom(double zoomVel, double dt);
        
        void keys(const Uint8* keys,double dt);

};

class Point{
    private:
        tuple<int,int,int> color;

        
    public:
        int colliderID;
        collisionType collisType;
        RigidBody* rb;
        //references to components which point
        float diameter;
        void init(RigidBody* rb, tuple<int,int,int> color,float diameter, int colliderID = -1, collisionType = pointHitPoly);
        void render(Screen* screen);
        void changeColor(tuple<int,int,int> color);

        //collision callback functions 
        function<void(int, complex<double>)>   collisionCallback;
        function<int()>   getDamageCallback;
        // if uncalled, collision is disabled.
        void setCallBacks(function<void(int, complex<double>)> onCollision, function<int()> getDamage);
};  

class Polygon{
    private:
        //used when finding normals
        complex<double> rotNegative90;


        //transformations are applied to assetRE and written to assetWR
        vector<complex<double>> assetWR1;
        vector<complex<double>> assetWR2;
        
        double furthestDistance;
    public:
        int colliderID;
        //references to components which polygon depends on
        RigidBody* rb;
        int numVertices;
        float lineThickness;
        tuple<int,int,int> color;
        vector<double> alphas;
        //reference to a list of complex points, length depends on whats given when intitalized. Read only
        vector<complex<double>> assetRE;

        //how you access the current shape of the polygon
        vector<complex<double>>* currentAsset;
        vector<complex<double>>* nextAsset;

       
        double scale;
        
        //used for morphing the shape
        vector<complex<double>> vertexOffsets;

        //can be called during runtime, do so before update
        void appendPoint(complex<double> pnt);
        void loadAsset(vector<complex<double>>* asset, tuple<int,int,int> color);

        // both get normal and get smallest raduis use next asset because theyre used in collision
        complex<double> getNormal(int index);
        //get  center and radius of smallest circle which contains polygon
        tuple<double,complex<double>> getContainingCircle();

        void resetVertexOffsets();
        
        void init(vector<complex<double>>* asset, RigidBody* rb, tuple<int,int,int> color, int colliderID = -1);



        //updates assetWR using posistion and rotation values from RigidBody
        void update();
        void render(Screen* screen);

        //collision callback functions 
        function<void(int, complex<double>)>   collisionCallback;
        function<int()>   getDamageCallback;
        // if uncalled, collision is disabled.
        void setCallBacks(function<void(int, complex<double>)> onCollision, function<int()> getDamage);
};

class VisualPolygon{
    public:
        vector<complex<double>> assetRE;
        vector<complex<double>> currentAsset;

        RigidBody* rb;
        int numVertices;
        float lineThickness;
        tuple<int,int,int> color;
        vector<double> alphas;
        double scale;

        //used for morphing the shape
        vector<complex<double>> vertexOffsets;

        void resetVertexOffsets();

        //can be called during runtime, do so before update
        void appendPoint(complex<double> pnt);
        void loadAsset(vector<complex<double>>* asset, tuple<int,int,int> color);
        
        // used to move asset origin, used by joint in rigged poly
        // mutates assetRE!!
        void rebase(complex<double>);
        
        void init(vector<complex<double>>* asset, RigidBody* rb, tuple<int,int,int> color);
        
        //updates assetWR using posistion and rotation values from RigidBody
        void update();
        void render(Screen* screen);
};


class Trail{
    // vertices disappear when theyre timer runs out
    // when vertices disappear, they become the new head of the trail
    // the head of the trail follows the object until a new head vertex is chosen
    // only the head of the trail moves, all other vertices are fixed in place

    // segment thickness = (timeRemaining/decaytime)*startThickness

    // note decayTimer/numVertices must be > dt (seconds per frame). otherwise 2 segments will
    // decay in the same tick 
    private:
        tuple<int,int,int> headColor;
        tuple<int,int,int> tailColor;
        vector<double> alphas;
        vector<tuple<int,int,int>> segmentColors;
        int trailHeadIndex;

        int numVertices;
        double decayTime;

        

        vector<complex<double>> vertexPos;
        vector<double> vetexTimers;

        //references to components which polygon depends on (only read by this component)
        RigidBody* rb;
    public:
        double thickness;
        void init(RigidBody* rb,double thickness, int numVertices, double decayTime);
        void reset(tuple<int,int,int> headColor,tuple<int,int,int> tailColor);

        void update(double dt);
        void render(Screen* screen);
};