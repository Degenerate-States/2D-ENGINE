#include "collisionFuncs.h"


//theta-phi except wrappes around angle seem between 0 and 2phi, (note also returns sign)
double smallestAngle(double theta, double phi){
    double delta = abs(theta-phi);
    double oppSign = (-1)*(theta-phi) / delta;
    if (2*M_PI -delta > delta){
        return theta-phi;
    }
    else{
        return oppSign*(2*M_PI -delta);
    }
}

//private helper function
tuple<double,double>
getLineScalarAndT(
complex<double> point1, complex<double> point2,
complex<double> startLine1,complex<double> startLine2,
complex<double> endLine1,complex<double> endLine2){

    complex<double> v = point2 - point1;
    complex<double> deltaP = point1 - startLine1;
    complex<double> q = endLine1 - startLine1;
    complex<double> l = startLine2-startLine1;
    complex<double> deltaQ = (endLine2 - startLine2) - q;

    double lineScalar = -1;
    double t = -1;

    //linear case
    if (abs(deltaQ)<reallySmall){
        //division by zero protection
        if(abs(real(q-v))>reallySmall){
            double denom = real(l)*(imag(v)+imag(q)) + imag(l)*(real(q) - real(v));
            //division by zero protection
            if (abs(denom)>reallySmall){
                lineScalar = (imag(deltaP)*(real(q)-real(v)) + real(deltaP)*(imag(v)-imag(q)))/denom;
                t = (real(deltaP) - lineScalar*real(l))/(real(q)-real(v));
            }
        }else{
            //division by zero protection
            if(abs(imag(q-v))>reallySmall){
                double denom = imag(l)*(real(v)+real(q)) + real(l)*(imag(q) - imag(v));
                //division by zero protection
                if (abs(denom)>reallySmall){
                    lineScalar = (real(deltaP)*(imag(q)-imag(v)) + imag(deltaP)*(real(v)-real(q)))/denom;
                    t = (imag(deltaP) - lineScalar*imag(l))/(imag(q)-imag(v));
                }
            }
        }

    
    //quadratic case
    }else{
        //form a quadratic for finding scalar corrisponding to where on the line was hit
        double a = - real(deltaQ)*imag(l) + real(l)*imag(deltaQ);
        double b = real(deltaQ)*imag(deltaP) - real(l)*imag(v) + real(v)*imag(l) - real(q)*imag(l) + real(l)*imag(q) - real(deltaP)*imag(deltaQ);
        double c = -real(v)*imag(deltaP) + real(q)*imag(deltaP) + real(deltaP)*imag(v) -real(deltaP)* imag(q);

        double t1 = -1;
        double t2 = -1;

        double denom1;
        double denom2;

        //quadratic case
        double determinant = b*b -4.0*a*c;

        if (determinant > 0){
            double lineScalar1 = (-b + sqrt(determinant))/(2*a);
            double lineScalar2 = (-b - sqrt(determinant))/(2*a);

            double denom1 = -real(v)+ real(q) +real(deltaQ)*lineScalar1;
            double denom2 = -real(v)+ real(q) +real(deltaQ)*lineScalar2;

            // if either denom is zero then the respective t will remain as -1
            if (abs(denom1) > reallySmall){
                t1 = (real(deltaP) - lineScalar1*real(l))/denom1;
            }
            if (abs(denom2) > reallySmall){
                t2 = (real(deltaP) - lineScalar2*real(l))/denom2;
            }

            //negative t values are irrelevent
            if (t1 < 0){
                t1= 100;
            }
            if (t2 < 0){
                t2 = 100;
            }

            if(t2>t1){
                t = t1;
                lineScalar = lineScalar1;

            }else{
                if(t1>t2){
                    t = t2;
                    lineScalar = lineScalar2;
                }
            }
        }
    }
    // if all of the above fail, t will be left as -1 as will lineScalar
    return make_tuple(t,lineScalar);
}


// private helper fuction
tuple<bool, double ,complex<double>> 
willPointHitLine 
(complex<double> point1, complex<double> point2, 
complex<double> startLine1,complex<double> startLine2, 
complex<double> endLine1,complex<double> endLine2){

    tuple<double,double> returnVal = getLineScalarAndT(point1,point2,startLine1,startLine2,endLine1,endLine2);

    double t = get<0>(returnVal);
    double lineScalar = get<1>(returnVal);

    if (t > 0 && t < 1+pntCollisionPadTemporal){
        if (lineScalar > -pntCollisionPadSpatial && lineScalar < 1+pntCollisionPadSpatial){
            complex <double> collisionPoint = point1 + (point2 - point1)*t;
            return make_tuple(true,t,collisionPoint);
        }
    }
    //all but false is dummy data
    return make_tuple(false,t,point1);
}

// TODO: circle of influence calculation does not accomidate for previous vertex posisitons
// this can be changed in poly->getContainingCircle(), however it would double the calculations done in there
// pointCollisionPadSpatial could be increased to accomidate for this instead
// furthermore a more efficent system for pre collision checking could be used (ie sector system)
tuple<bool,int,complex<double>> 
willBulletHitPoly(Polygon* poly,Bullet* bullet, int polyID,double dt){
    complex<double> bulletPos = bullet->prevPos;
    complex<double> bulletNextPos = bullet->rb.pos;

    bool collisionOccured = false;
    complex<double> collisionPoint= bulletPos;
    double minT = 2;
    int minIndex = -1;


    //checks if bullet is active and poly isnt the one who fired it
    if(bullet->rb.active && polyID != bullet->shooterID){
        // checks if bullet will be in circle containing polygon
        tuple<double,complex<double>> polyCircle = poly->getContainingCircle();
        double possibleCollisonRad = abs(get<1>(polyCircle) - bulletNextPos) + pntCollisionPadSpatial;
        if (possibleCollisonRad < get<0>(polyCircle)){

                tuple<bool, double ,complex<double>> helperReturnVal;
                //updated if a line was hit, updated further if another line was hit with smaller t
                int iPlus1;
                for(int i = 0; i < poly->currentAsset->size(); i++){
                
                    iPlus1 = (i+1)%poly->currentAsset->size();

                    helperReturnVal = willPointHitLine (bulletPos, bulletNextPos, 
                                                        (*poly->currentAsset)[i],(*poly->currentAsset)[iPlus1], 
                                                        (*poly->nextAsset)[i],(*poly->nextAsset)[iPlus1]);

                    // checks if line was hit, and if was hit before other hit lines
                    if (get<0>(helperReturnVal) && get<1>(helperReturnVal) < minT){
                        collisionOccured = true;
                        minT = get<1>(helperReturnVal);
                        minIndex = i;
                        collisionPoint = get<2>(helperReturnVal);
                    }
                }
    
        }// end of if collision is possilbe

    }// end of if bullet is active
    return make_tuple(collisionOccured, minIndex, collisionPoint);
}

// change this to do distance checking if get smallest distance method doesnt have to recalculate
bool isPointInPoly(Polygon* poly,complex<double> point){
    double totalRot = 0;

    double thetaI;
    double thetaIPlus1;

    for(int i = 0; i < poly->numVertices; i++){
        thetaI = arg((*poly->nextAsset)[i] - point);
        thetaIPlus1 = arg((*poly->nextAsset)[(i+1)%poly->numVertices] - point);
        totalRot += smallestAngle(thetaIPlus1,thetaI);
    }
    // total rot will be either 0 or 2pi, we return false if zero, and true if 2pi
    return abs(totalRot) > M_PI;
}

complex<double>
reflectAboutNormal(complex<double> normal, complex<double> vec){
    double dotProd = real(normal)*real(vec)+imag(normal)*imag(vec);
    vec += 2*abs(dotProd)*normal;
    return vec;
}