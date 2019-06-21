#pragma once

#include "Vector.h"

using namespace std;

// Class for vector circles.
class Circle {
public:
    double radius;
    Vector centre;

    // Constructor for the circle.
    Circle(double rad, Vector cent) {
        this->radius = rad;
        this->centre = cent;
    }
};
