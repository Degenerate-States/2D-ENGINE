#ifndef config_h
#define config_h
#include <tuple>
#include <stdint.h>
using namespace std;
//must be known at compile time
constexpr int bulletPoolSize = 5;

//used inside functions
constexpr double pntCollisionPadSpatial = 0.05;
constexpr double pntCollisionPadTemporal = 0.05;
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


    //bullet config

    //bullet trail
    //time in seconds
    double bulletTrailDecay;
    // must be greater than 2
    int bulletTrailSegments;
    void init();
};
#endif