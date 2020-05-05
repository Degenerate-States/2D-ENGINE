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
        std::complex<double> pos;
        std::complex<double> vel;

        //in radians
        double rot;
        double rotVel;

        //complex used to rotate other complex number by rot though multiplication
        //note multiplying by its complex conj will be the negative of this rotation
        //rotOp is updated in update method, if its going to be used update method must be called
        std::complex<double> rotOp;
        
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
        tuple<double,double> worldToScreen(std::complex<double> val);
        std::complex<double> screenToWorld(double x, double y);
        std::complex<double> pixelScreenToWorld(int x, int y);

        //inc or dec by zoomVel*dt 
        void changeZoom(double zoomVel, double dt);
        
        void keys(const Uint8* keys,double dt);

};

class Point{
    private:
        tuple<int,int,int> color;
        
    public:
        float diameter;
        void init(tuple<int,int,int> color,float diameter);
        void render(Screen* screen,RigidBody* rb);
        void changeColor(tuple<int,int,int> color);
};  
class Polygon{
    private:
        tuple<int,int,int> color;
        float thickness;

        //used when finding normals
        std::complex<double> rotNegative90;


        //reference to a list of complex points, length depends on whats given when intitalized. is read only
        std::vector<std::complex<double>> assetRE;

        

    public:
        //transformations are applied to assetRE and written to assetWR
        std::vector<std::complex<double>> assetWR;
        // smallest radius circle which contains poly
        double smallestRadius;
        double scale;
        //can be called during runtime, do so before update
        void appendPoint(std::complex<double> pnt);
        void loadAsset(std::vector<std::complex<double>>* asset, tuple<int,int,int> color);
        void init(std::vector<std::complex<double>>* asset,tuple<int,int,int> color);

        std::complex<double> getNormal(int index);

        //updates assetWR using posistion and rotation values from RigidBody
        void update(RigidBody* rb);
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

        std::vector<complex<double>> vertexPos;
        std::vector<double> vetexTimers;
    public:
        void init(int numVertices,double decayTime);
        void reset(RigidBody* rb,double thickness, 
                    tuple<int,int,int> headColor,tuple<int,int,int> tailColor);

        void update(RigidBody* rb,double dt);
        void render(Screen* screen);
};
#endif
