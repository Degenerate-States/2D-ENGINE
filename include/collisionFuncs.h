#ifndef collisionFuncs_h
#define collisionFuncs_h
#include <complex>
#include <cmath>
#include <tuple>
#include <vector>
#include "components.h"
#include "projectileManager.h"
#include "config.h"


//returns collision info on the first line in the polygon hit between now and next frame
tuple<bool,int,complex<double>> 
willBulletHitPoly(Polygon* poly,Bullet* bullet,RigidBody* polyRB,int polyID,double dt);

// returns true if point is in polygon
bool isPointInPoly(Polygon* poly,complex<double> point);

// for riccochet application
complex<double>
reflectAboutNormal(complex<double> normal, complex<double> vec);


#endif