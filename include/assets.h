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
        std::vector<std::complex<double>> gunAsset;

        //where all the assets are acutally defined
        void init();
};

#endif