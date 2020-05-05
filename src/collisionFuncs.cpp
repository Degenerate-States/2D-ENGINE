#include <complex>
#include <cmath>
#include <tuple>
#include <vector>
#include "config.h"
#include "components.h"
#include "collisionFuncs.h"
using namespace std;

tuple<bool,std::complex<double>> 
doLineSegmentsIntersect 
(std::complex<double> p1, std::complex<double> p2,
std::complex<double> q1,std::complex<double> q2)
{
    double scalar;
    std::complex<double> intersection;
    std::complex<double> divisor = q2-q1-p2+p1;

    if (std::real(divisor)>std::imag(divisor)){
        scalar = std::real(p1-q1)/std::real(divisor);
    }
    else{
        scalar = std::imag(p1-q1)/std::imag(divisor);
    }
    
    if ((scalar>0.0-pointCollisionPadding) && (scalar<1.0+pointCollisionPadding)){
        intersection = p1 + scalar*(p2-p1);
        return make_tuple(true,intersection);
    }
    else{
        //divisor is just dummy data here
        return make_tuple(false,divisor);
    }
}

tuple<bool,int,std::complex<double>> 
doesLineIntersectPoly(Polygon* poly,std::complex<double> linStart,std::complex<double> linEnd){

    tuple<bool,std::complex<double>> helperReturnVal;
    int iPlus1;
    for(int i = 0; i < poly->assetWR.size(); i++){

        iPlus1 = (i+1)%poly->assetWR.size();
        helperReturnVal = doLineSegmentsIntersect(poly->assetWR[i], poly->assetWR[iPlus1],linStart,linEnd);

        if (get<0>(helperReturnVal)){
            return make_tuple(true,i,get<1>(helperReturnVal));
        }
    }
    //everything other than false is dummy data
    return make_tuple(false,0,get<1>(helperReturnVal));
}