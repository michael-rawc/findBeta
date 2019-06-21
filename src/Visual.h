#pragma once

#include <fstream>
#include <string>
#include "Agent.h"

using namespace std;

// Class for the visualisation.
class Visual {
public:
    Agent agent;
    vector<vector<Hold>> grid;
    array<array<int, 2>, 4> climberLocations;

    // Constructor for the visualisation.
    Visual(Agent a) {
        agent = a;
        grid = this->agent.wall.grid;
        climberLocations[0] = this->agent.climber.leftHand;
        climberLocations[1] = this->agent.climber.rightHand;
        climberLocations[2] = this->agent.climber.leftFoot;
        climberLocations[3] = this->agent.climber.rightFoot;

        printOut();
    }

    // Function to update the visualisation.txt.
    void updateVisual(Agent a) {
        this->agent = a;
        this->climberLocations[0] = this->agent.climber.leftHand;
        this->climberLocations[1] = this->agent.climber.rightHand;
        this->climberLocations[2] = this->agent.climber.leftFoot;
        this->climberLocations[3] = this->agent.climber.rightFoot;

        printOut();
    }

    // Function to clear visualisation.txt
    void clear() {
        // Setting up ofstream
        ofstream visualisation;
        visualisation.open("visualisation.txt");
        visualisation << endl;
        // Closing ofstream
        visualisation.close();
    }
private:

    // Function to print wall and climber to visualisation.txt
    void printOut() {
        // Setting up ofstream
        ofstream visualisation;
        visualisation.open("visualisation.txt");

        for (int j = grid.size()-1; j >= 0; j--) {
            string currentline = "";
            for (int i = 0; i < grid[j].size(); i++) {
                // Print to visualisation.txt
                if (grid[j][i].isHold) {
                    if (grid[j][i].location[0] == climberLocations[0][0] && grid[j][i].location[1] == climberLocations[0][1]) {
                        // Left hand is on this hold
                        currentline += 'L';
                    }
                    else if (grid[j][i].location[0] == climberLocations[1][0] && grid[j][i].location[1] == climberLocations[1][1]) {
                        // Right hand is on this hold
                        currentline += 'R';
                    }
                    else if (grid[j][i].location[0] == climberLocations[2][0] && grid[j][i].location[1] == climberLocations[2][1]) {
                        // Left foot is on this hold
                        currentline += 'l';
                    }
                    else if (grid[j][i].location[0] == climberLocations[3][0] && grid[j][i].location[1] == climberLocations[3][1]) {
                        // Right foot is on this hold
                        currentline += 'r';
                    }
                    else {
                        // No extremities on hold, just a hold
                        currentline += 'H';
                    }
                }
                else {
                    currentline += ' ';
                }
            }

            visualisation << currentline << endl;
        }

        visualisation.close();
    }
};
