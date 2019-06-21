#pragma once

#include "Vector.h"

using namespace std;

// Class to model holds on the wall.
class Hold {
public:
    int type;
    bool isHold;
    bool isStartHand;
    bool isStartFoot;
    bool isEnd;
    array<int, 2> location;

    // Constructor for all the different types of holds.
    Hold(int t, array<int, 2> loc) {
        this->type = t;
        this->location = loc;

        if (type == 0) { // No hold
            //make empty 'hold'
            this->isHold = false;
            this->isStartHand = false;
            this->isStartFoot = false;
            this->isEnd = false;
        }
        else if (type == 1) { // Hold is a starting hand default
            //make default start 'hold'
            this->isHold = true;
            this->isStartHand = true;
            this->isStartFoot = false;
            this->isEnd = false;
        }
        else if (type == 2) { // Hold is a starting foot default
            //make default start 'hold'
            this->isHold = true;
            this->isStartHand = false;
            this->isStartFoot = true;
            this->isEnd = false;
        }
        else if (type == 3) { // Hold is of the default
            //make default 'hold'
            this->isHold = true;
            this->isStartHand = false;
            this->isStartFoot = false;
            this->isEnd = false;
        }
        else if (type == 4) { // Hold is a ending default
            //make default 'hold'
            this->isHold = true;
            this->isStartHand = false;
            this->isStartFoot = false;
            this->isEnd = true;
        }
    }

    // Default Constructor.
    Hold() {

    }

    // Function to return the vector position of a hold.
    Vector position() {
        return(Vector((this->location[0] + 0.5) / 10, (this->location[1] + 0.5) /10)); // in metres
    }
};
