#ifndef assets_h
#define assets_h
#include "components.h"
#include <complex>
#include <vector>
using namespace std;

class Assets{
    public:
        // list of all game assets
        std::vector<std::complex<double>> plrAsset;

        //where all the assets are acutally defined
        void init();
        
        //tools for loading them into different components
        //pass the component and which asset from assets you wish to load
        void polygonLoader(Polygon* poly, std::vector<std::complex<double>>* asset);
};

#endif