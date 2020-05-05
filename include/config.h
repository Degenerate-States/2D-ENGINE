#ifndef config_h
#define config_h
#include <tuple>
#include <stdint.h>
using namespace std;
//must be known at compile time
constexpr int bulletPoolSize = 5;

//used inside functions
constexpr double pointCollisionPadding = 0.05;

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