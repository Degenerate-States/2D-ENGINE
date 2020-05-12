#ifndef riggedPoly_h
#define riggedPoly_h
#include "components.h"

class Joint{
    private:
        Polygon* poly;
    public:
        RigidBody rb;
        tuple<int,int,int> color;

        // indices of vertexes puppeted by joint
        vector<int> attachedVerts;

        void update(double dt);

};
class Skeleton{
    public:
        Polygon poly;
        vector<Joint*> joints;

        void update(double dt);
        void render(Screen* screen);
};


#endif