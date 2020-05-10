#include "assets.h"
using namespace std;

void Assets::init(){
    //polygon asset must be defined clockwise, otherwise normals will be inverted
    this->plrAsset = {{-0.05,-0.04},{0.08,0.0},{-0.05,0.04},{-0.02,-0.0}};
    this->gunAsset= {{0.06,0.0},{0.01,0.01},{-0.0,0.02},{-0.02,-0.0},{0.0,-0.02},{0.01,-0.01}};
    this->flameAsset = {{-0.12,-0.0},{-0.05,-0.02},{-0.03,-0.0},{-0.05,0.02}};
    this->box = {{-0.07,-0.08},{0.09,-0.08},{0.09,0.08},{-0.07,0.08}};
}