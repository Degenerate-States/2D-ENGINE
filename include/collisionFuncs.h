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

//returns collision info on the first line in the polygon hit between now and next frame
tuple<bool,int,complex<double>> 
willPointHitPoly(Polygon* poly, complex<double> bulletPos, complex<double> bulletNextPos);

// returns true if point is in polygon
bool isPointInPoly(Polygon* poly,complex<double> point);

// for riccochet application
complex<double>
reflectAboutNormal(complex<double> normal, complex<double> vec);

void pointPolyCollision(Polygon* poly,Point* pnt,collisionType type);