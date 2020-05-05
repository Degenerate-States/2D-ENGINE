#ifndef collisionFuncs_h
#define collisionFuncs_h
#include <complex>
#include <cmath>
#include <tuple>
#include <vector>
#include "components.h"

//returns: do intersect, intersect point
tuple<bool,std::complex<double>> 
doLineSegmentsIntersect 
(std::complex<double> p1, std::complex<double> p2,
std::complex<double> q1,std::complex<double> q2);



//returns: do intersect, index of poly, intersect point
tuple<bool,int,std::complex<double>> 
doesLineIntersectPoly
(Polygon* poly,std::complex<double> linStart,std::complex<double> linEnd);

#endif