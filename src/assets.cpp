#include "components.h"
#include "assets.h"
#include <complex>
#include <vector>
#include <iostream>
using namespace std;

void Assets::init(){
    this->plrAsset = {{0.05,-0.0},{-0.05,-0.04},{-0.03,0.0},{-0.05,0.04}};
}
void Assets::polygonLoader(Polygon* poly, std::vector<std::complex<double>>* asset){
    for(int i = 0; i < asset->size(); i++){
        poly->appendPoint((*asset)[i]);
    }
}