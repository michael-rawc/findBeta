#pragma once

#include "Circle.h"
#include "Line.h"
#include "State.h"
#include "Action.h"

using namespace std;

// Class for the agent.
class Agent {
public:
    bool hasTerminated;
    bool completed;
    State currentState;
    vector<Action> currentPossibleActions;
    Wall wall;
    Climber climber;
    int previousExtemityMoved;
    Hold previousHoldMovedFrom;

    // Constructor for the agent.
    Agent(Wall w, Climber c) {
        this->previousExtemityMoved = 4;
        this->previousHoldMovedFrom = Hold();
        this->wall = w;
        this->climber = c;
        this->currentState = State(climber);
        this->currentPossibleActions = this->getActions();
        this->hasTerminated = false;
        this->completed = false;
        // cout << "STATE! Actions available: " << currentPossibleActions.size() << endl;
        // for (int i = 0; i < currentPossibleActions.size(); i++) {
        //     cout << " " << currentPossibleActions[i].extremity << " to "
        //         << currentPossibleActions[i].toLocation[0] << ","
        //         << currentPossibleActions[i].toLocation[1] << endl;
        // }
    }

    // Default Constructor.
    Agent() {

    }

    // Function to execute an action.
    void executeAction(Action action, bool verbose) {
        if (verbose) {
            cout << "Number of actions available1: " << this->currentPossibleActions.size() << endl;
        }
        // Update previous move info
        this->previousExtemityMoved = action.extremity;
        this->previousHoldMovedFrom = this->climber.holdAt(this->wall, action.extremity);

        // Move climber to hold specified by action
        this->climber.move(action.extremity, action.toLocation);

        // Update agent state and available actions
        this->updateAgent();

        if (verbose) {
            cout << " ACTION! Moved " << action.extremity << " to " << action.toLocation[0] << "," << action.toLocation[1] << endl;
            cout << "Number of actions available2: " << this->currentPossibleActions.size() << endl;
        }
    }
private:

    // Function to update the agent.
    void updateAgent() {
        // Update state and actions list
        this->currentState = State(this->climber);
        this->currentPossibleActions = this->getActions();
        // cout << "STATE! Actions available: " << currentPossibleActions.size() << endl;
        // for (int i = 0; i < currentPossibleActions.size(); i++) {
        //     cout << " " << currentPossibleActions[i].extremity << " to "
        //         << currentPossibleActions[i].toLocation[0] << ","
        //         << currentPossibleActions[i].toLocation[1] << endl;
        // }

        // Testing if the updated agent is now in a terminal state
        if (this->currentPossibleActions.size() == 0) {
            // cout << "updateAgent: no actions available, somehow - terminated" << endl;
            this->hasTerminated = true;
        }
        else if (this->isFinished()) {
            // cout << "updateAgent: climber has completed the problem - terminated" << endl;
            this->completed = true;
            this->hasTerminated = true;
        }
    }

    // Function to get a vector of all possible actions.
    vector<Action> getActions() {
        vector<Action> actions;
        vector<Hold> holds = this->getHolds();
        // cout << "getActions: size of holds to test: " << holds.size() << endl;

        for (int i = 0; i < holds.size(); i++) { // For each hold on problem
            for (int j = 0; j < 4; j++) { // For each extremity
                if (this->isPossibleMove(j, holds[i])) {
                    actions.push_back(Action(j, holds[i]));
                }
            }
        }

        // Check to ensure previous move is removed from available actions
        if (this->previousExtemityMoved != 4) {
            Action undoAction = Action(this->previousExtemityMoved, this->previousHoldMovedFrom);
            // If find returns the end, did not find the element in the vector
            if (find(actions.begin(), actions.end(), undoAction) != actions.end()) {
                actions.erase(remove(actions.begin(), actions.end(), undoAction), actions.end());
            }
        }

        return(actions);
    }

    // Function to get a vector of all the holds on the wall.
    vector<Hold> getHolds() {
        vector<Hold> holds;
        Hold leftHand = this->climber.holdAt(wall, 0);
        Hold rightHand = this->climber.holdAt(wall, 1);
        Hold leftFoot = this->climber.holdAt(wall, 2);
        Hold rightFoot = this->climber.holdAt(wall, 3);
        // For each hold in the grid
        for (int j = this->wall.grid.size()-1; j >= 0; j--) {
            for (int i = 0; i < this->wall.grid[j].size(); i++) {
                // Testing if the hold is a hold and if it is not a hold the climber is already on
                if (this->wall.grid[j][i].isHold) {
                    if ((this->wall.grid[j][i].location[0] == leftHand.location[0] && this->wall.grid[j][i].location[1] == leftHand.location[1])
                    || (this->wall.grid[j][i].location[0] == rightHand.location[0] && this->wall.grid[j][i].location[1] == rightHand.location[1])
                    || (this->wall.grid[j][i].location[0] == leftFoot.location[0] && this->wall.grid[j][i].location[1] == leftFoot.location[1])
                    || (this->wall.grid[j][i].location[0] == rightFoot.location[0] && this->wall.grid[j][i].location[1] == rightFoot.location[1])) {
                        // Climber is already on this hold
                        // cout << "getHolds: climber is already on hold " << i << "," << j << endl;
                    }
                    else {
                        holds.push_back(this->wall.grid[j][i]);
                    }
                }
            }
        }

        return(holds);
    }

    // Function to indicate if a desired move is possible or not.
    bool isPossibleMove(int extremity, Hold hold) {
        // cout << "attempting move for " << extremity << " to " << hold.location[0] << "," << hold.location[1] << endl;

        // Reach
        vector<double> distances;
        for (int i = 0; i < 4; i++) {
            if (i != extremity) {
                // Find the distance between the current hold and the desired hold
                distances.push_back(distanceBetweenHolds(this->climber.holdAt(wall, i), hold));
            }
            else {
                // That distance is 0 and irrelevant, but this is needed for indexing
                distances.push_back(0);
            }
        }
        // Get the maximum
        int furthestExtremity = getMaxIndex(distances);
        return(this->checkReach(extremity, furthestExtremity, hold));
    }

    // Function to return the distance between two given holds.
    double distanceBetweenHolds(Hold hold1, Hold hold2) {
        array<double, 2> hold1Pos = {(((double)hold1.location[0] + 0.5) / 10), (((double)hold1.location[1] + 0.5) / 10)}; // in metres
        array<double, 2> hold2Pos = {(((double)hold2.location[0] + 0.5) / 10), (((double)hold2.location[1] + 0.5) / 10)};
        double base = hold2Pos[0] - hold1Pos[0];
        double height = hold2Pos[1] - hold1Pos[1];

        return(sqrt(pow(base, 2) + pow(height, 2)));
    }

    // Function to get max index from a vector, if multuple maxima, return a the first one.
    int getMaxIndex(vector<double> values) {
        vector<int> maxIndexes;
        vector<double> maxValues;

        // Checks for vector being of poor sizing
        if (values.size() == 1) {
            return(0);
        }
        else if (values.size() == 0) {
            cout << "getMaxIndex: vector has size 0" << endl;
            return(0);
        }
        else {
            // Assign initial maximal value from first comparison
            if (values[1] > values[0]) {
                maxValues.push_back(values[1]);
                maxIndexes.push_back(1);
            }
            else {
                maxValues.push_back(values[0]);
                maxIndexes.push_back(0);
            }

            // Past the first comparison, find any new maxima
            for (int i = 1; i < values.size(); i++) {
                // If new maximal, clear vectors and add new values
                if (values[i] > maxValues[0]) {
                    maxValues.clear();
                    maxValues.push_back(values[i]);
                    maxIndexes.clear();
                    maxIndexes.push_back(i);
                }
                // If value is the same as current maxima, add to vector
                else if (values[i] == maxValues[0]) {
                    maxValues.push_back(values[i]);
                    maxIndexes.push_back(i);
                }
            }

            // Return first index from maxIndexes
            return(maxIndexes[0]);
        }
    }

    // Function to check if the climber can reach a hold.
    bool checkReach(int extremity, int furthestExtremity, Hold hold) {
        //bool isReachPossible = false;
        // The four circle method
        if (extremity == 0) { // Want to move left hand
            vector<Circle> circles;
            circles.push_back(Circle(this->climber.maxArmReach, Vector(hold.position().value[0], hold.position().value[1])));
            circles.push_back(Circle(this->climber.maxArmReach, Vector(this->climber.holdAt(this->wall, 1).position().value[0], this->climber.holdAt(this->wall, 1).position().value[1])));
            circles.push_back(Circle(this->climber.maxLegReach, Vector(this->climber.holdAt(this->wall, 2).position().value[0], this->climber.holdAt(this->wall, 2).position().value[1])));
            circles.push_back(Circle(this->climber.maxLegReach, Vector(this->climber.holdAt(this->wall, 3).position().value[0], this->climber.holdAt(this->wall, 3).position().value[1])));
            // Find furthest extremity's position
            if (furthestExtremity >= 0 && furthestExtremity <= 3) {
                Vector furthestExtremityPosition = Vector(this->climber.holdAt(this->wall, furthestExtremity).position().value[0], this->climber.holdAt(this->wall, furthestExtremity).position().value[1]);
                // Find direction to desired hold from furthest extremity position
                Vector direction = furthestExtremityPosition.directionTo(hold.position());

                // Make the four lines for the joints
                vector<Line> lines;
                lines.push_back(Line(this->climber.jointPosition(furthestExtremityPosition, furthestExtremity, 0), direction));
                lines.push_back(Line(this->climber.jointPosition(furthestExtremityPosition, furthestExtremity, 1), direction));
                lines.push_back(Line(this->climber.jointPosition(furthestExtremityPosition, furthestExtremity, 2), direction));
                lines.push_back(Line(this->climber.jointPosition(furthestExtremityPosition, furthestExtremity, 3), direction));
                // isReachPossible = isTValid(circles, lines);
                return(isTValid(circles, lines));
            }
            else {
                cout << "checkReach: furthestExtremity invalid" << endl;
                return(false);
            }
        }
        else if (extremity == 1) { // Want to move right hand
            vector<Circle> circles;
            circles.push_back(Circle(this->climber.maxArmReach, Vector(this->climber.holdAt(this->wall, 0).position().value[0], this->climber.holdAt(this->wall, 0).position().value[1])));
            circles.push_back(Circle(this->climber.maxArmReach, Vector(hold.position().value[0], hold.position().value[1])));
            circles.push_back(Circle(this->climber.maxLegReach, Vector(this->climber.holdAt(this->wall, 2).position().value[0], this->climber.holdAt(this->wall, 2).position().value[1])));
            circles.push_back(Circle(this->climber.maxLegReach, Vector(this->climber.holdAt(this->wall, 3).position().value[0], this->climber.holdAt(this->wall, 3).position().value[1])));
            // Find furthest extremity's position
            if (furthestExtremity >= 0 && furthestExtremity <= 3) {
                Vector furthestExtremityPosition = Vector(this->climber.holdAt(this->wall, furthestExtremity).position().value[0], this->climber.holdAt(this->wall, furthestExtremity).position().value[1]);
                // Find direction to desired hold from furthest extremity position
                Vector direction = furthestExtremityPosition.directionTo(hold.position());

                // Make the four lines for the joints
                vector<Line> lines;
                lines.push_back(Line(this->climber.jointPosition(furthestExtremityPosition, furthestExtremity, 0), direction));
                lines.push_back(Line(this->climber.jointPosition(furthestExtremityPosition, furthestExtremity, 1), direction));
                lines.push_back(Line(this->climber.jointPosition(furthestExtremityPosition, furthestExtremity, 2), direction));
                lines.push_back(Line(this->climber.jointPosition(furthestExtremityPosition, furthestExtremity, 3), direction));
                // isReachPossible = isTValid(circles, lines);
                return(isTValid(circles, lines));
            }
            else {
                cout << "checkReach: furthestExtremity invalid" << endl;
                return(false);
            }
        }
        else if (extremity == 2) { // Want to move left foot
            vector<Circle> circles;
            circles.push_back(Circle(this->climber.maxArmReach, Vector(this->climber.holdAt(this->wall, 0).position().value[0], this->climber.holdAt(this->wall, 0).position().value[1])));
            circles.push_back(Circle(this->climber.maxArmReach, Vector(this->climber.holdAt(this->wall, 1).position().value[0], this->climber.holdAt(this->wall, 1).position().value[1])));
            circles.push_back(Circle(this->climber.maxLegReach, Vector(hold.position().value[0], hold.position().value[1])));
            circles.push_back(Circle(this->climber.maxLegReach, Vector(this->climber.holdAt(this->wall, 3).position().value[0], this->climber.holdAt(this->wall, 3).position().value[1])));
            // Find furthest extremity's position
            if (furthestExtremity >= 0 && furthestExtremity <= 3) {
                Vector furthestExtremityPosition = Vector(this->climber.holdAt(this->wall, furthestExtremity).position().value[0], this->climber.holdAt(this->wall, furthestExtremity).position().value[1]);
                // Find direction to desired hold from furthest extremity position
                Vector direction = furthestExtremityPosition.directionTo(hold.position());

                // Make the four lines for the joints
                vector<Line> lines;
                lines.push_back(Line(this->climber.jointPosition(furthestExtremityPosition, furthestExtremity, 0), direction));
                lines.push_back(Line(this->climber.jointPosition(furthestExtremityPosition, furthestExtremity, 1), direction));
                lines.push_back(Line(this->climber.jointPosition(furthestExtremityPosition, furthestExtremity, 2), direction));
                lines.push_back(Line(this->climber.jointPosition(furthestExtremityPosition, furthestExtremity, 3), direction));
                // isReachPossible = isTValid(circles, lines);
                return(isTValid(circles, lines));
            }
            else {
                cout << "checkReach: furthestExtremity invalid" << endl;
                return(false);
            }
        }
        else if (extremity == 3) { // Want to move right foot
            vector<Circle> circles;
            circles.push_back(Circle(this->climber.maxArmReach, Vector(this->climber.holdAt(this->wall, 0).position().value[0], this->climber.holdAt(this->wall, 0).position().value[1])));
            circles.push_back(Circle(this->climber.maxArmReach, Vector(this->climber.holdAt(this->wall, 1).position().value[0], this->climber.holdAt(this->wall, 1).position().value[1])));
            circles.push_back(Circle(this->climber.maxLegReach, Vector(this->climber.holdAt(this->wall, 2).position().value[0], this->climber.holdAt(this->wall, 2).position().value[1])));
            circles.push_back(Circle(this->climber.maxLegReach, Vector(hold.position().value[0], hold.position().value[1])));
            // Find furthest extremity's position
            if (furthestExtremity >= 0 && furthestExtremity <= 3) {
                Vector furthestExtremityPosition = Vector(this->climber.holdAt(this->wall, furthestExtremity).position().value[0], this->climber.holdAt(this->wall, furthestExtremity).position().value[1]);
                // Find direction to desired hold from furthest extremity position
                Vector direction = furthestExtremityPosition.directionTo(hold.position());

                // Make the four lines for the joints
                vector<Line> lines;
                lines.push_back(Line(this->climber.jointPosition(furthestExtremityPosition, furthestExtremity, 0), direction));
                lines.push_back(Line(this->climber.jointPosition(furthestExtremityPosition, furthestExtremity, 1), direction));
                lines.push_back(Line(this->climber.jointPosition(furthestExtremityPosition, furthestExtremity, 2), direction));
                lines.push_back(Line(this->climber.jointPosition(furthestExtremityPosition, furthestExtremity, 3), direction));
                // isReachPossible = isTValid(circles, lines);
                return(isTValid(circles, lines));
            }
            else {
                cout << "checkReach: furthestExtremity invalid" << endl;
                return(false);
            }
        }
        else {
            cout << "checkReach: extremity invalid" << endl;
            return(false);
        }

        //return(isReachPossible);
    }

    // Function to return if there exists a valid t value.
    bool isTValid(vector<Circle> circles, vector<Line> lines) {
        // Intersection testing on each pair (circle, line)
        vector<array<double, 2>> tBoundaries;
        // For each pair
        if (circles.size() == 4 && lines.size() == 4) {
            for (int i = 0; i < 4; i++) {
                if (doesLineIntersectCircle(circles[i], lines[i])) {
                    tBoundaries.push_back(getTBoundaries(circles[i], lines[i]));
                }
                else {
                    // One of the lines doens't intersect the circle, therefore entire move is impossible
                    // cout << "isTValid: one of the lines doesn't intersect its circle" << endl;
                    return(false);
                }
            }
        }
        else {
            cout << "isTValid: size of circles or lines invalid" << endl;
        }

        // Testing if there exists a t value within the boundaries
        bool noMin = true;
        bool noMax = true;
        double minAllowed;
        double maxAllowed;
        for (int i = 0; i < 4; i++) {
            if (noMin) {
                minAllowed = tBoundaries[i][0];
                noMin = false;
            }
            if (noMax) {
                maxAllowed = tBoundaries[i][1];
                noMax = false;
            }
            if (tBoundaries[i][0] > minAllowed) {
                minAllowed = tBoundaries[i][0];
            }
            if (tBoundaries[i][1] < maxAllowed) {
                maxAllowed = tBoundaries[i][1];
            }
        }
        // Check if there can exist a t value
        if (minAllowed <= maxAllowed) {
            return(true);
            // cout << "isTValid: t boundaries are legal" << endl;
        }
        else {
            return(false);
            // cout << "isTValid: t boundaries are not legal" << endl;
        }
    }

    // Function to return any t values where a line intersects a circle.
    bool doesLineIntersectCircle(Circle circle, Line line) {
        // Finding vector to the start of the line from the centre of the circle
        Vector circleToStart = line.start.minus(circle.centre);

        // Using discriminant to see if the line intersects the circle
        double b = 2 * line.direction.dot(circleToStart);
        double c = circleToStart.dot(circleToStart) - pow(circle.radius, 2);
        double discriminant = pow(b, 2) - (4 * c);

        if (discriminant > 0) {
            // Does intersect the circle
            return(true);
        }
        else {
            return(false);
        }
    }

    // Function to return the t values at intersection.
    array<double, 2> getTBoundaries(Circle circle, Line line) {
    array<double, 2> ts;
        // Finding vector to the start of the line from the centre of the circle
        Vector circleToStart = line.start.minus(circle.centre);

        // Using discriminant to see if the line intersects the circle
        double b = 2 * line.direction.dot(circleToStart);
        double c = circleToStart.dot(circleToStart) - pow(circle.radius, 2);
        double discriminant = pow(b, 2) - (4 * c);

        if (discriminant > 0) {
            // Does intersect the circle
            double t1 = (-b + sqrt(discriminant)) / 2;
            double t2 = (-b - sqrt(discriminant)) / 2;
            if (t1 <= t2) {
                ts = {t1, t2};
            }
            else {
                ts = {t2, t1};
            }
        }

        return(ts); // Will never be null as we confirmed the line does intersect
    }

    // Function to test if the climber has finished the problem.
    bool isFinished() {
        // Test if hand extremities are on finish holds
        if ((this->climber.leftHand[0] == this->wall.finishHold[0] && this->climber.leftHand[1] == this->wall.finishHold[1])
           || (this->climber.rightHand[0] == this->wall.finishHold[0] && this->climber.rightHand[1] == this->wall.finishHold[1])) {
            return(true);
        }
        else {
            return(false);
        }
    }
};
