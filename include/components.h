#ifndef components_h
#define components_h
#include <complex>
#include <vector>
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

        void setPos(double x, double y);
        //displace should be avoided for constant movement as it doesnt depend on dt (hence will vary with fps)
        void displace(double dx, double dy);

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

        //inc or dec by zoomVel*dt 
        void changeZoom(double zoomVel, double dt);

};


class Polygon{
    private:
        uint8_t red;
        uint8_t green;
        uint8_t blue;

        double scale;
        //reference to a list of complex points, length depends on whats given when intitalized
        std::vector<std::complex<double>> assetRE;
        //assetRE is read only, transformations are applied to it and written here
        std::vector<std::complex<double>> assetWR;
    public:
        //append point can be used during game loop, however it should be called before update
        void appendPoint(std::complex<double> pnt);
        void init(uint8_t r,uint8_t g,uint8_t b);
        //updates assetWR using posistion and rotation values from RigidBody
        void update(RigidBody* rb);
        void render(Screen* screen);
};

#endif
