#ifndef config_h
#define config_h
#include <tuple>
#include <stdint.h>
using namespace std;

//must be known at compile time
constexpr int bulletPoolSize = 5;
constexpr int sparkPoolSize = 10;

//global line thickness
constexpr float defaultLineThickness = 4.0;

//used inside functions
constexpr double pntCollisionPadSpatial = 0;
constexpr double pntCollisionPadTemporal = 0;
constexpr double reallySmall = 1.0e-100;

constexpr int windowSizeX =  1400;
constexpr int windowSizeY = 800;
constexpr tuple<int,int,int> white = make_tuple(255,255,255);
constexpr tuple<int,int,int> red = make_tuple(255,0,0);
constexpr tuple<int,int,int> orange = make_tuple(255, 127, 0);

struct Config{
    int fps;

    //calculated
    int mspf;
    double spf;

    void init();
};

//game specific
struct Stats{
    //appearence

    //bullets
        //time in seconds
    double bulletTrailDecay;
        // must be greater than 2
    int bulletTrailSegments;
    double bulletDiameter;
    
    //sparks
    double sparkDiameter;
    int sparkTrailSegments;
    double sparkTrailDecay;
    //divides riccochet vel by this to get num sparks
    double riccoSparkSpawnDamping;
    //divides riccochet vel by this to get spark vel
    double riccoSparkVelDamping;
    double sparkVelVarience;

    //controls screen motion based on current controls
    double screenVel;
    double screenOffset;
    double defaultZoom;

    //gameplay
    double plrTopSpeed;
    double plrAcceleration;
    double plrBulletVel;
    double plrShotVarience;

    void init();
};
#endif