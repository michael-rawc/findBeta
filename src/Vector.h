#pragma once

#include <iostream>
#include <array>
#include <math.h>

using namespace std;

// Class for 2D vectors.
class Vector {
public:
    array<double, 2> value;

    // Constructor for the vector.
    Vector(double x, double y) {
        this->value = {x, y};
    }

    // Default Constructor.
    Vector() {

    }

    // Function to get the vector direction to the desired hold.
    Vector directionTo(Vector holdPosition) {
        // Vector direction = holdPosition.minus(Vector(this->value[0], this->value[1])); // A->B = B-A
        Vector direction = holdPosition.minus(*this); // A->B = B-A
        direction = direction.normalise();

        return(direction);
    }

    // Function to calculate the dot product of two 2D vectors.
    double dot(Vector vector2) {
        double result = (this->value[0] * vector2.value[0]) + (this->value[1] * vector2.value[1]);

        return(result);
    }

    // Function to multiply a vector by a scalar.
    Vector multiply(double scalar) {
        Vector result = Vector(scalar * this->value[0], scalar * this->value[1]);

        return(result);
    }

    // Function to minus two 2D vectors.
    Vector minus(Vector vector2) {
        Vector result = Vector(this->value[0] - vector2.value[0], this->value[1] - vector2.value[1]);

        return(result);
    }

    // Function to minus two 2D vectors.
    Vector add(Vector vector2) {
        Vector result = Vector(this->value[0] + vector2.value[0], this->value[1] + vector2.value[1]);

        return(result);
    }

    // Function to find the magnitude of a 2D vector.
    double magnitude() {
        double magnitude = sqrt(pow(this->value[0], 2) + pow(this->value[1], 2));

        return(magnitude);
    }

    // Function to normalise a 2D vector.
    Vector normalise() {
        double magnitude = this->magnitude();
        Vector direction = Vector(this->value[0] / magnitude, this->value[1] / magnitude);

        return(direction);
    }
};
