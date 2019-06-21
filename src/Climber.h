#pragma once

#include "Wall.h"

using namespace std;

// Class to model the climber.
class Climber {
public:
    array<int, 2> leftHand;
    array<int, 2> rightHand;
    array<int, 2> leftFoot;
    array<int, 2> rightFoot;
    double mass;
    double height;
    double apeIndex;
    double torsoHeight;
    double torsoWidth;
    double maxArmReach; // Maximum length the arm can reach
    double maxLegReach; // Maximum length the leg can reach
    Vector leftShoulder; // Position of the left shoulder with respect to the torso origin
    Vector rightShoulder; // Position of the right shoulder with respect to the torso origin
    Vector leftHip; // Position of the left hip with respect to the torso origin
    Vector rightHip; // Position of the right hip with respect to the torso origin


    // Constructor for the climber.
    Climber(Wall wall, array<int, 2> lh, array<int, 2> rh, array<int, 2> lf, array<int, 2> rf, double m, double h, double aI) {
        this->leftHand = lh;
        this->rightHand = rh;
        this->leftFoot = lf;
        this->rightFoot = rf;
        this->mass = m;
        this->height = h / 100;
        this->apeIndex = aI;
        double shoulderWidth = this->height / 3.74; // From average male statistics https://www.firstinarchitecture.co.uk/average-male-and-female-dimensions/
        this->torsoHeight = this->height / 2.60; // ^ (hand height == hip height) (ignoring head) from ^
        this->maxArmReach = ((this->height * this->apeIndex) - shoulderWidth) / 2; // Calculating maximum reach from ape index
        this->maxLegReach = this->height - this->torsoHeight;
        this->torsoWidth = ((this->height / 3.74) + (this->height / 4.83)) / 2; // Average shoulder width and hip width from ^
        // Joint locations, based on cuboid torso representation
        this->leftShoulder = Vector(0, this->torsoHeight);
        this->rightShoulder = Vector(this->torsoWidth, this->torsoHeight);
        this->leftHip = Vector(0, 0);
        this->rightHip = Vector(this->torsoWidth, 0);

        cout << "--Climber initiated, mass: " << mass << "kg, height: " << height
            << "m, arm reach: " << maxArmReach <<"m, leg reach: " << maxLegReach << "m" << endl;
        cout << "   starting at: " << leftHand[0] << "," << leftHand[1] <<
            "; " << rightHand[0] << "," << rightHand[1] <<
            "; " << leftFoot[0] << "," << leftFoot[1] <<
            "; " << rightFoot[0] << "," << rightFoot[1] << endl;
    }

    // Deafault Constructor.
    Climber() {
        
    }

    // Function to move an extremity from one hold to another.
    void move(int extremity, array<int, 2> toLocation) {
        if (extremity == 0) {
            this->leftHand = toLocation;
        }
        else if (extremity == 1) {
            this->rightHand = toLocation;
        }
        else if (extremity == 2) {
            this->leftFoot = toLocation;
        }
        else if (extremity == 3) {
            this->rightFoot = toLocation;
        }
        else {
            cout << "move: extremity invalid" << endl;
        }
    }

    // Function to return the hold at an extremity.
    Hold holdAt(Wall wall, int extremity) {
        if (extremity == 0) { // Left hand
            if (this->leftHand[0] < wall.grid[0].size() && this->leftHand[1] < wall.grid.size()) {
                return(wall.grid[this->leftHand[1]][this->leftHand[0]]);
            }
            else {
                //cout << "holdAt: left hand is not on wall" << endl;

                return(Hold(0, {-1, -1}));
            }
        }
        else if (extremity == 1) { // Right hand
            if (this->rightHand[0] < wall.grid[0].size() && this->rightHand[1] < wall.grid.size()) {
                return(wall.grid[this->rightHand[1]][this->rightHand[0]]);
            }
            else {
                //cout << "holdAt: right hand is not on wall" << endl;

                return(Hold(0, {-1, -1}));
            }
        }
        else if (extremity == 2) { // Left foot
            if (this->leftFoot[0] < wall.grid[0].size() && this->leftFoot[1] < wall.grid.size()) {
                return(wall.grid[this->leftFoot[1]][this->leftFoot[0]]);
            }
            else {
                //cout << "holdAt: left foot is not on wall" << endl;

                return(Hold(0, {-1, -1}));
            }
        }
        else if (extremity == 3) { // Right foot
            if (this->rightFoot[0] < wall.grid[0].size() && this->rightFoot[1] < wall.grid.size()) {
                return(wall.grid[this->rightFoot[1]][this->rightFoot[0]]);
            }
            else {
                //cout << "holdAt: right foot is not on wall" << endl;

                return(Hold(0, {-1, -1}));
            }
        }
        else {
            cout << "holdAt: extremity invalid" << endl;
            return(Hold(0, {-1, -1}));
        }
    }

    // Function to return the position of a specified joint.
    Vector jointPosition(Vector furthestLimbJointPosition, int from, int to) {
        Vector result = furthestLimbJointPosition.add(limbDifference(from, to));

        return(result);
    }

    // Function to get the vector from from joint to to joint.
    Vector limbDifference(int from, int to) {
        if (from == 0) {
            if (to == 0) { // From leftShoulder to leftShoulder
                return(this->leftShoulder.minus(this->leftShoulder));
            }
            else if (to == 1) { // From leftShoulder to rightShoulder
                return(this->rightShoulder.minus(this->leftShoulder));
            }
            else if (to == 2) { // From leftShoulder to leftHip
                return(this->leftHip.minus(this->leftShoulder));
            }
            else if (to == 3) { // From leftShoulder to rightHip
                return(this->rightHip.minus(this->leftShoulder));
            }
            else {
                cout << "limbDifference: to invalid" << endl;
                Vector thing = Vector(-1, -1);

                return(thing);
            }
        }
        else if (from == 1) {
            if (to == 0) { // From rightShoulder to leftShoulder
                return(this->leftShoulder.minus(this->rightShoulder));
            }
            else if (to == 1) { // From rightShoulder to rightShoulder
                return(this->rightShoulder.minus(this->rightShoulder));
            }
            else if (to == 2) { // From rightShoulder to leftHip
                return(this->leftHip.minus(this->rightShoulder));
            }
            else if (to == 3) { // From rightShoulder to rightHip
                return(this->rightHip.minus(this->rightShoulder));
            }
            else {
                cout << "limbDifference: to invalid" << endl;
                Vector thing = Vector(-1, -1);

                return(thing);
            }
        }
        else if (from == 2) {
            if (to == 0) { // From leftHip to leftShoulder
                return(this->leftShoulder.minus(this->leftHip));
            }
            else if (to == 1) { // From leftHip to rightShoulder
                return(this->rightShoulder.minus(this->leftHip));
            }
            else if (to == 2) { // From leftHip to leftHip
                return(this->leftHip.minus(this->leftHip));
            }
            else if (to == 3) { // From leftHip to rightHip
                return(this->rightHip.minus(this->leftHip));
            }
            else {
                cout << "limbDifference: to invalid" << endl;
                Vector thing = Vector(-1, -1);

                return(thing);
            }
        }
        else if (from == 3) {
            if (to == 0) { // From rightHip to leftShoulder
                return(this->leftShoulder.minus(this->rightHip));
            }
            else if (to == 1) { // From rightHip to rightShoulder
                return(this->rightShoulder.minus(this->rightHip));
            }
            else if (to == 2) { // From rightHip to leftHip
                return(this->leftHip.minus(this->rightHip));
            }
            else if (to == 3) { // From rightHip to rightHip
                return(this->rightHip.minus(this->rightHip));
            }
            else {
                cout << "limbDifference: to invalid" << endl;
                Vector thing = Vector(-1, -1);

                return(thing);
            }
        }
        else {
            cout << "limbDifference: from invalid" << endl;
            Vector thing = Vector(-1, -1);

            return(thing);
        }
    }
};
