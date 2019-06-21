#pragma once

#include "Vector.h"

using namespace std;

// Class for vector lines.
class Line {
public:
    Vector start;
    Vector direction;

    // Constructor for the infinite line.
    Line(Vector s, Vector d) {
        this->start = s;
        this->direction = d;
    }

};
