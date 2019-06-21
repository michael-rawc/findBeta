#pragma once

#include "Hold.h"

using namespace std;

// Class to model actions for the agent.
class Action {

public:
    int extremity;
    array<int, 2> toLocation;

    // Constructor for the action.
    Action(int extrem, Hold nextHold) {
        this->extremity = extrem;
        this->toLocation = nextHold.location;
    }
};

bool operator== (const Action &action1, const Action &action2) {
    if (action1.extremity == action2.extremity &&
        action1.toLocation[0] == action2.toLocation[0] &&
        action1.toLocation[1] == action2.toLocation[1]) {
        return(true);
    }
    else {
        return(false);
    }
}
