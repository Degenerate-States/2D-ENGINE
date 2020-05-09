#ifndef components_h
#define components_h
#include <complex>
#include <vector>
#include <tuple>
#include "SDL.h"
using namespace std;


class RigidBody{
    public:
        double mass;
        complex<double> pos;
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

        //references to components which polygon depends on (only read by this component)
        RigidBody* rb;
        
    public:
        float diameter;
        void init(RigidBody* rb, tuple<int,int,int> color,float diameter);
        void render(Screen* screen);
        void changeColor(tuple<int,int,int> color);
};  
class Polygon{
    private:
        tuple<int,int,int> color;
        float thickness;

        //used when finding normals
        complex<double> rotNegative90;


        //reference to a list of complex points, length depends on whats given when intitalized. is read only
        vector<complex<double>> assetRE;


        //transformations are applied to assetRE and written to assetWR
        vector<complex<double>> assetWR1;
        vector<complex<double>> assetWR2;
        
        //references to components which polygon depends on (only read by this component)
        RigidBody* rb;
        
    public:
        //how you access the current shape of the polygon
        vector<complex<double>>* currentAsset;
        vector<complex<double>>* nextAsset;

       
        double scale;
        
        //used for morphing the shape
        vector<complex<double>> vertexOffsets;

        //can be called during runtime, do so before update
        void appendPoint(complex<double> pnt);
        void loadAsset(vector<complex<double>>* asset, tuple<int,int,int> color);
        void init(vector<complex<double>>* asset, RigidBody* rb, tuple<int,int,int> color);


        // both get normal and get smallest raduis use next asset because theyre used in collision
        complex<double> getNormal(int index);
        //get radius of smallest circle which contains polygon
        double getSmallestRadius();

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

        vector<tuple<int,int,int>> segmentColors;
        int trailHeadIndex;

        int numVertices;
        double decayTime;

        double startThickness;

        vector<complex<double>> vertexPos;
        vector<double> vetexTimers;

        //references to components which polygon depends on (only read by this component)
        RigidBody* rb;
    public:
        void init(RigidBody* rb, int numVertices, double decayTime);
        void reset(double thickness, tuple<int,int,int> headColor,tuple<int,int,int> tailColor);

        void update(double dt);
        void render(Screen* screen);
};
#endif
