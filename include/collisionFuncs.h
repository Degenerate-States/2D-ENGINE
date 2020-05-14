#pragma once
#include "components.h"
#include "config.h"

#include <complex>
#include <cmath>
#include <tuple>
#include <vector>

enum collisionType{ pointHitPoly, pointInPoly, polyPoly };

//theta-phi except wrappes around angle seem between 0 and 2phi, (note also returns sign)
double smallestAngle(double theta, double phi);

// for riccochet application
complex<double>
reflectAboutNormal(complex<double> normal, complex<double> vec);

void pointPolyCollision(Polygon* poly,Point* pnt,collisionType type);

void polyPolyCollision(Polygon* poly1, Polygon* poly2,collisionType type);