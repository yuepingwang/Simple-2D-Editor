//
// Created by Yueping Wang on 11/5/17.
//

#ifndef ASSIGNMENT2_TRIANGLES_H
#define ASSIGNMENT2_TRIANGLES_H
#include "Helpers.h"
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <Eigen/Dense>

typedef Eigen::Matrix<float,2,3> twoByThree;
typedef Eigen::Matrix<float,3,3> threeByThree;

class Triangle{
private:
    double a, b, c;//parametization of three lines
    //the distance from cursor location to each vertex location,
    Eigen::Vector3f dp0,dp1,dp2;
//    Eigen::Vector2f p0,p1,p2;
    Eigen::Vector3f barycenter;
    double angle = M_PI/36;
    Eigen::Matrix3f rC;
    Eigen::Matrix3f rCC;
    Eigen::Matrix3f zIn;
    Eigen::Matrix3f zOut;

public:
    Triangle();
    Triangle(threeByThree);
    threeByThree V;
    threeByThree Vc;//for interpolating colors
    bool checkIntersection(double,double);
    //get distance between cursor and vertices, only if triangle is selected
    void calculateDist(double,double);
    //called in the "while" loop of main function, only if triangle is selected
    void updateVertices(double,double);
    void rotateT(int &,int &, int);
    void scaleT(int &,int &, int);
    void changeColor(int, int);
    int getClosestVertex();
    void worldToWindow(threeByThree &,Eigen::Matrix3f);
    void windowToWorld(threeByThree &,Eigen::Matrix3f);
    ~Triangle(){
        //
    };
};


#endif //ASSIGNMENT2_TRIANGLES_H
