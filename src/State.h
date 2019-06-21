#pragma once

#include "Climber.h"

using namespace std;

// Class to model the environment as a state.
class State {

public:
    array<int, 2> leftHand;
    array<int, 2> rightHand;
    array<int, 2> leftFoot;
    array<int, 2> rightFoot;

    // Constructor for the climber.
    State(Climber climber) {
        this->leftHand = climber.leftHand;
        this->rightHand = climber.rightHand;
        this->leftFoot = climber.leftFoot;
        this->rightFoot = climber.rightFoot;
    }

    // Default Constructor. 
    State() {

    }
};

bool operator== (const State &state1, const State &state2) {
    if (state1.leftHand[0] == state2.leftHand[0] &&
        state1.leftHand[1] == state2.leftHand[1] &&
        state1.rightHand[0] == state2.rightHand[0] &&
        state1.rightHand[1] == state2.rightHand[1] &&
        state1.leftFoot[0] == state2.leftFoot[0] &&
        state1.leftFoot[1] == state2.leftFoot[1] &&
        state1.rightFoot[0] == state2.rightFoot[0] &&
        state1.rightFoot[1] == state2.rightFoot[1]) {
        return(true);
    }
    else {
        return(false);
    }
}
