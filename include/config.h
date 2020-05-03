#ifndef config_h
#define config_h
struct Config{
    int windowSizeX;
    int windowSizeY;
    int fps;

    //calculated
    int mspf;
    double spf;


    void init();
};
#endif