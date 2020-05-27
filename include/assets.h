#pragma once

#include "components.h"
#include <complex>
#include <vector>

// used to serialize rigged poly joint data
struct jointInfo{
    vector<int> indices;
    complex<double> pos;

    vector<complex<double>> visualAsset;
    //member initalizer
    jointInfo(vector<int> indices,complex<double> pos,vector<complex<double>> visualAsset = {}) : 
                indices(indices), pos(pos), visualAsset(visualAsset){}
};
// used by skeleton to store info on joint connnections
struct linkInfo{
    vector<int> indices;
    vector<double> rigidity;
    linkInfo(vector<int> indices,vector<double> rigidity) :  indices(indices), rigidity(rigidity){}

};

class Assets{
    private:
        int colliderID;
    public:
        // list of all game assets
        vector<complex<double>> plrAsset;
        vector<complex<double>> gunAsset;
        
        vector<complex<double>> flameAsset;
        vector<jointInfo> flameJoints;
        vector<linkInfo> flameLinks;

        vector<complex<double>> box;

        vector<complex<double>> swarmerAsset;

        vector<complex<double>> outerEngBall;
        vector<complex<double>> innerEngBall;

        vector<complex<double>> testPoly;
        vector<jointInfo> testJoints;

        vector<complex<double>> testSnakePoly;
        vector<jointInfo> testSnakeJoints;

        vector<complex<double>> testEye;

        vector<complex<double>> hostPoly;
        vector<jointInfo> hostJoints;
        vector<linkInfo> hostLinks;

        vector<complex<double>> skeleTestPoly;
        vector<jointInfo> skeleTestJoints;
        vector<linkInfo> skeleTestLinks;

            
        vector<complex<double>> sunPoly;
        vector<jointInfo> sunJoints;
        vector<linkInfo> sunLinks;
        //where all the assets are acutally defined
        void init();

        int getID();
};