#pragma once

#include <vector>
#include "Hold.h"

using namespace std;

// Class to model the wall.
class Wall {
public:
    double width;
    double height;
    vector<vector<Hold>> grid;
    double angle;
    double frictionFoot;
    double frictionHand;
    array<int, 2> finishHold;

    // Constructor to make a wall with dimensions and angle to vertical.
    Wall(double w, double h, double a) {
        this->width = w;
        this->height = h;
        this->angle = a;
        this->grid = makeEmptyWall(width, height);

        cout << "--Wall initiated, size: " << width << "m x " <<
            height << "m, angle: " << angle <<
            " degrees" << endl;
    }

    // Deafult Constructor.
    Wall() {
        
    }

    void setFinishHold(array<int, 2> fh) {
        this->finishHold = fh;
    }


private:

    // Function to make grid into the correct size and fill with default holds.
    vector<vector<Hold>> makeEmptyWall(double w, double h) {
        vector<vector<Hold>> g; // Grid of 10cmx10cm holds
        vector<vector<Hold>> g2;
        for (int j = 0; j < (h * 10); j ++) {
            vector<Hold> gMini;
            for (int i = 0; i < (w * 10); i ++) {
                gMini.push_back(Hold(0, {i, j}));
            }
            g.push_back(gMini);
        }
        // Reverse the order so 0,0 is bottom left
        for (int i = g.size() - 1; i >= 0; i--) {
            g2.push_back(g[i]);
        }
        return(g2);
    }
};
