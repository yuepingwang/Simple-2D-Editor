//
// Created by Yueping Wang on 11/5/17.
//

#include "triangles.h"
#include <iostream>
#include <fstream>
#include "Helpers.h"
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <cmath>

using namespace std;
using namespace Eigen;
typedef Eigen::Matrix<float,2,3> twoByThree;
typedef Eigen::Matrix<float,3,3> threeByThree;

Triangle::Triangle(threeByThree _V)
{
    V = _V;
    barycenter<<(V.col(0)+V.col(1)+V.col(2))/3;
    rC<<    cos(angle),-sin(angle),0,
            sin(angle),cos(angle),0,
            0,0,1;
    rCC<<   cos(-angle),-sin(-angle),0,
            sin(-angle),cos(-angle),0,
            0,0,1;
    zIn<<   1.25,0,0,
            0,1.25,0,
            0,0,1;
    zOut<<  0.75,0,0,
            0,0.75,0,
            0,0,1;
    Vc<<    0,0,0,
            0,0,0.1,
            0.4,0,0;
}
Triangle::Triangle() = default;

bool Triangle::checkIntersection(double worldX, double worldY){
    /* CHECK INTERSECTION USING BARYCENTRIC COORDINATE*/
    Vector3f xy((float)worldX,(float)worldY,1);
    Matrix3f Q;
    Q.row(0)<<V(0,0),V(0,1),V(0,2);
    Q.row(1)<<V(1,0),V(1,1),V(1,2);
    Q.row(2)<<1,1,1;

    Vector3f R = Q.colPivHouseholderQr().solve(xy);
    cout<<"R: "<< R<<endl;
    a = R(0);
    b = R(1);
    c = R(2);
    if(a>0 && b>0 && c>0)
        return true;
    else
        return false;
};

int Triangle::getClosestVertex(){
    /* CHECK INTERSECTION USING BARYCENTRIC COORDINATE*/
    cout<<"dist to p0 is: "<<dp0.norm()<<endl;
    cout<<"dist to p1 is: "<<dp1.norm()<<endl;
    cout<<"dist to p2 is: "<<dp2.norm()<<endl;
    if (dp0.norm()<dp1.norm()&& dp0.norm()<dp2.norm()) {
        return 0;
    }
    else if(dp0.norm()<dp1.norm()&& dp0.norm()>dp2.norm()){
        return 2;
    }
    else if(dp0.norm()>dp1.norm()&& dp2.norm()>dp1.norm()){
        return 1;
    }
    else return 2;
};

void Triangle::calculateDist(double worldX, double worldY) {
    dp0<<(V(0,0)-worldX), (V(1,0)-worldY), 0;
    dp1<<(V(0,1)-worldX), (V(1,1)-worldY), 0;
    dp2<<(V(0,2)-worldX), (V(1,2)-worldY), 0;
}

void Triangle::updateVertices(double worldX, double worldY) {
    V.col(0)<<dp0(0)+worldX, dp0(1)+worldY, 1;
    V.col(1)<<dp1(0)+worldX, dp1(1)+worldY, 1;
    V.col(2)<<dp2(0)+worldX, dp2(1)+worldY, 1;
}

void Triangle::scaleT(int &winW,int &winH,int zoom){
    barycenter<<(V.col(0)+V.col(1)+V.col(2))/3;
    Matrix3f trans;
    trans <<1,0,-barycenter(0),
            0,1,-barycenter(1),
            0,0,1;
    V = trans*V;
    if(zoom == 0){
        V = zIn*V;
    }
    else{
        V = zOut*V;
    }
    V = (trans.inverse())*V;
    cout<<"SCALE UP OR DOWN!"<<endl;
}


void Triangle::rotateT(int &winW,int &winH,int direction){

    barycenter<<(V.col(0)+V.col(1)+V.col(2))/3;
    Matrix3f trans;
    trans <<1,0,-barycenter(0),
            0,1,-barycenter(1),
            0,0,1;
    V = trans*V;
    if(direction == 0){
        V = rC*V;
    }
    else{
        V = rCC*V;
    }
    V = trans.inverse()*V;
    cout<<"ROTATE 10 DEGREES!"<<endl;
}

void Triangle::changeColor(int i, int v) {
    Vector3f pointC;
    switch(i){
        case 1:
            pointC<<0.2,0.3,0.5;
            break;
        case 2:
            pointC<<0,1,0;
            break;
        case 3:
            pointC<<0,0,1;
            break;
        case 4:
            pointC<<0.5,0.5,0;
            break;
        case 5:
            pointC<<0,0.5,0.5;
            break;
        case 6:
            pointC<<0.5,0,0.5;
            break;
        case 7:
            pointC<<0.1,0.7,1;
            break;
        case 8:
            pointC<<1,0.3,0.1;
            break;
        case 9:
            pointC<<0.2,1,1;
            break;
        default:
            break;
    }
    switch(v){
        case 0:
            Vc.col(0)<<pointC;
            break;
        case 1:
            Vc.col(1)<<pointC;
            break;
        case 2:
            Vc.col(2)<<pointC;
            break;
        default:
            break;
    }

}
