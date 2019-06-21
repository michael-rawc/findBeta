#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "QAgent.h"
#include "Visual.h"

using namespace std;

// Declaring helper functions.
Wall loadProblem(string);
Climber loadClimber(Wall, string);
bool startsWith(string, string);
bool endsWith(string, string);

// Main method.
int main(int argc, char * argv[]) {
    if (argc != 3) {
        cout << "Incorrect number of arguments for findBeta" << endl;
    }

    string climberFile = argv[1];
    string problemFile = argv[2];

    // Load the problem to a wall
    Wall wall = loadProblem(problemFile);
    // Load the climber with the wall
    Climber climber0 = loadClimber(wall, climberFile); // Initial climber

    // Seed RNG
    srand(time(NULL));

    // -- Find beta!
    vector<Action> foundBeta;

    // Simulation(s) settings
    int agents = 1; // Number of agents to run
    int episodes = 200; // Number of episodes to run
    int maxAllowedMoves = 1000; // Number of moves allowed before deemed a failure
    int numberOfTests = 50; // Number of tests to perform over the episodes
    int testInterval = (int) episodes / numberOfTests; // Based on ^ do not edit
    bool visualiseAtEnd = true; // Whether to visualise or not
    int numberOfVisualisations = 100; // Number of times to show the visualisation

    // Variables
    vector<vector<int>> allAgentScores;
    double alpha = 0.9;
    double gamma = 0.9;
    double epsilon = 0.5;
    cout << "Starting parameters:" << endl;
    cout << " alpha: " << alpha << ", gamma: " << gamma << ", epsilon: " << epsilon << endl;
    cout << agents << " agent(s) with " << episodes << " episodes" << endl;

    // For each agent
    for (int agentNum = 0; agentNum < agents; agentNum ++) {
        cout << "Agent " << agentNum << " has started training" << endl;
        vector<int> thisAgentScores;

        // Initialise Q-Learning agent
        QAgent qAgent = QAgent(Agent(wall, climber0), alpha, gamma, epsilon);

        for (int episodeNum = 0; episodeNum < episodes; episodeNum ++) {
            // Decaying epsilon and alpha values
            qAgent.epsilon = epsilon - (epsilon * (episodeNum / episodes));
            qAgent.alpha = alpha - (alpha * (episodeNum / episodes));

            // Initialise new episode
            qAgent.agent = Agent(wall, climber0);

            int numOfMovesForEpisode = 0;

            // Loop till simulation terminates
            while(!qAgent.agent.hasTerminated && numOfMovesForEpisode < maxAllowedMoves) {
                // Store state and actions
                State state = qAgent.agent.currentState;
                vector<Action> actions = qAgent.agent.currentPossibleActions;

                // Choose an action based on policy derived from Q table
                Action chosenAction = qAgent.getEGreedyAction();

                // Execute action chosen
                qAgent.agent.executeAction(chosenAction, false);
                numOfMovesForEpisode ++;
                int reward = -1;

                // If last moved terminated the simulation, adjust reward
                if (qAgent.agent.hasTerminated) {
                    if (qAgent.agent.completed) {
                        // Got to the finish hold
                        reward += 10000;
                    }
                    // Failed
                    else {
                        // Ran out of possible moves
                        reward -= 10000;
                    }
                }

                // Update Q table pair value
                qAgent.updateQ(state, chosenAction, qAgent.agent.currentState, qAgent.agent.currentPossibleActions, (double) reward);
            }

            if (qAgent.agent.completed) {
                cout << " Agent " << agentNum << " episode " << episodeNum << " completed(succeeded in " << numOfMovesForEpisode << " moves)" << endl;
            }
            else {
                cout << " Agent " << agentNum << " episode " << episodeNum << " completed(failed in " << numOfMovesForEpisode << " moves)" << endl;
            }

            // Tests
            if (((episodeNum+1) % testInterval) == 0 || episodeNum == 0) {
                cout << "Q table has size: " << qAgent.qTable.size() << endl;
                cout << " --test started";

                // Initialise new episode
                qAgent.agent = Agent(wall, climber0);

                int numOfMovesForTest = 0;

                // Loop till simulation terminates
                while(!qAgent.agent.hasTerminated && numOfMovesForTest < maxAllowedMoves) {
                    // Choose the best action
                    Action chosenAction = qAgent.getBestAction();

                    // Execute action chosen
                    qAgent.agent.executeAction(chosenAction, false);
                    numOfMovesForTest ++;
                }

                if (qAgent.agent.completed) {
                    cout << "\r" << " --test completed(succeeded in " << numOfMovesForTest << " moves)" << endl;
                    // Add score to vector
                    thisAgentScores.push_back(numOfMovesForTest);
                }
                else {
                    cout << "\r" << " --test completed(failed in " << numOfMovesForTest << " moves)" << endl;
                    // Add score to vector
                    thisAgentScores.push_back(maxAllowedMoves);
                }
            }
        }

        // After all episodes done for this agent, add test scores to vector for all agents
        allAgentScores.push_back(thisAgentScores);

        // On the last agent, store a list of the found beta
        if (agentNum == agents-1) {
            // Initialise new episode
            qAgent.agent = Agent(wall, climber0);

            int numOfMovesForBeta = 0;

            // Loop till simulation terminates
            while(!qAgent.agent.hasTerminated && numOfMovesForBeta < maxAllowedMoves) {
                // Choose the best action
                Action chosenAction = qAgent.getBestAction();
                // Store chosen action
                foundBeta.push_back(chosenAction);
                numOfMovesForBeta ++;

                // Execute action chosen
                qAgent.agent.executeAction(chosenAction, false);
            }
        }

    }

    // Print out test scores
    cout << "--Test scores--" << endl;
    if (agents > 0) {
        for (int i = 0; i < agents; i++) {
            cout << " Agent " << i << ":" << endl;
            cout << "  ";
            for (int j = 0; j < allAgentScores[i].size(); j++) {
                if (allAgentScores[i][j] != maxAllowedMoves) {
                    cout << allAgentScores[i][j] << " ";
                }
                else {
                    cout << "fail ";
                }
            }
            cout << endl;
        }
    }
    else {
        cout << "No agents" << endl;
    }

    // Calculate mean scores across agents and print results
    cout << " Mean of agents:" << endl;
    if (agents > 0) {
        vector<int> totalScoreAcrossAgents(allAgentScores[0].size());
        // Total up scores
        for (int i = 0; i < agents; i++) {
            for (int j = 0; j < totalScoreAcrossAgents.size(); j++) {
                // Add to running total
                totalScoreAcrossAgents[j] += allAgentScores[i][j];
            }
        }
        // Average scores and print results
        cout << "  ";
        for (int i = 0; i < totalScoreAcrossAgents.size(); i++) {
            // Add to running total
            totalScoreAcrossAgents[i] = totalScoreAcrossAgents[i] / agents;
            cout << totalScoreAcrossAgents[i] << " ";
        }
        cout << endl;
    }
    else {
        cout << "No agents" << endl;
    }

    // Print out chosen beta!
    cout << "--Sequence calculated--" << endl;
    for (int i = 0; i < foundBeta.size(); i++) {
        cout << " " << i+1 << ". Move ";
        if (foundBeta[i].extremity == 0) {
            cout << "left hand ";
        }
        else if (foundBeta[i].extremity == 1) {
            cout << "right hand ";
        }
        else if (foundBeta[i].extremity == 2) {
            cout << "left foot ";
        }
        else if (foundBeta[i].extremity == 3) {
            cout << "right foot ";
        }
        cout << "to hold at " << foundBeta[i].toLocation[0] << "," << foundBeta[i].toLocation[1] << endl;
    }

    // Visualise!
    if (visualiseAtEnd) {
        cout << "--Visualisation of calculated sequence--" << endl;
        cout << " see visualisation.txt for live updating beta" << endl;

        for (int i = 0; i < numberOfVisualisations; i++) {
            // Initialise new episode
            Agent agent = Agent(wall, climber0);
            // Initialise new visualisation
            Visual visual = Visual(agent);

            int numOfMovesForBeta = 0;

            // Loop till simulation terminates
            while(!agent.hasTerminated && numOfMovesForBeta < maxAllowedMoves) {
                // Get action from foundBeta
                Action chosenAction = foundBeta[numOfMovesForBeta];
                numOfMovesForBeta ++;

                // Execute action chosen
                agent.executeAction(chosenAction, false);
                // Update visualisation
                visual.updateVisual(agent);

                // Wait some time inbetween printing moves for clarity (requires c++11)
                this_thread::sleep_for(chrono::milliseconds(1000));
            }

            visual.clear();
        }
    }
}

// Helper functions.

// Function to load the problem to the wall.
Wall loadProblem(string problemFile) {
    vector<string> problem;
    bool noWall = false;
    // Load the problem from .txt
    if (endsWith(problemFile, ".txt")) {
        ifstream inFile;
        string tempLine;
        inFile.open("./" + problemFile);
        if (inFile.is_open()) {
            while(getline(inFile, tempLine)) {
                problem.push_back(tempLine);
            }
        }
        else {
            cout << "problem file failed to open" << endl;
            noWall = true;
        }
    }
    else {
        cout << "problem file does not end in .txt" << endl;
        noWall = true;
    }

    array<double, 5> wallProp;

    if (startsWith(problem[0], "wall")) {
        string tempLine = problem[0];
        int propCount = 0;
        // Removing "wall " from line
        tempLine = tempLine.substr(5, tempLine.size()-5);
        tempLine += ' '; // appending ' ' to line
        string tempNum = "";
        for (int k = 0; k < tempLine.size(); k++) {
            if (tempLine[k] != ' ') {
                tempNum += tempLine[k];
            }
            else {
                // Add current tempNum to a variable
                wallProp[propCount] = stod(tempNum);
                propCount += 1;
                // Reset tempNum
                tempNum = "";
            }
        }
    }
    else {
        cout << "no wall properties found in " << problemFile << endl;
        noWall = true;
    }

    if (!noWall) {
        // Intialise wall with found wallProp
        Wall wall = Wall(wallProp[0], wallProp[1], wallProp[2]);

        // Add the problem to the wall
        for (int j = 1; j < problem.size(); j++) {
            string tempLine = problem[j];
            int problemHeight = problem.size()-1;
            if (startsWith(problem[j], "0") || startsWith(problem[j], "1")
                || startsWith(problem[j], "2") || startsWith(problem[j], "3")
                || startsWith(problem[j], "4")) {
                for (int i = 0; i < tempLine.size(); i++) {
                    if (tempLine[i] == '1') {
                        wall.grid[problemHeight - j][i] = Hold(1, {i, (problemHeight - j)});
                    }
                    else if (tempLine[i] == '2') {
                        wall.grid[problemHeight - j][i] = Hold(2, {i, (problemHeight - j)});
                    }
                    else if (tempLine[i] == '3') {
                        wall.grid[problemHeight - j][i] = Hold(3, {i, (problemHeight - j)});
                    }
                    else if (tempLine[i] == '4') {
                        wall.grid[problemHeight - j][i] = Hold(4, {i, (problemHeight - j)});
                        array<int, 2> finishHold = {i, problemHeight - j};
                        wall.setFinishHold(finishHold);
                    }
                }
            }
            else {
                cout << "extra row of problem found in " << problemFile << endl;
            }
        }

        return(wall);
    }
    else {
        return(Wall());
    }
}

// Function to load the climber with the right starting hold(s).
Climber loadClimber(Wall wall, string climberFile) {
    vector<string> climber;
    bool noClimber = false;
    // Load the problem from .txt
    if (endsWith(climberFile, ".txt")) {
        ifstream inFile;
        string tempLine;
        inFile.open("./" + climberFile);
        if (inFile.is_open()) {
            while(getline(inFile, tempLine)) {
                climber.push_back(tempLine);
            }
        }
        else {
            cout << "climber file failed to open" << endl;
            noClimber = true;
        }
    }
    else {
        cout << "climber file does not end in .txt" << endl;
        noClimber = true;
    }

    array<double, 3> climberProp;
    for (int i = 0; i < climber.size(); i++) {
        if (startsWith(climber[i], "climber")) {
            string tempLine = climber[i];
            int propCount = 0;
            // Removing "climber " from line
            tempLine = tempLine.substr(8, tempLine.size()-8);
            tempLine += ' '; // appending ' ' to line
            string tempNum = "";
            for (int k = 0; k < tempLine.size(); k++) {
                if (tempLine[k] != ' ') {
                    tempNum += tempLine[k];
                }
                else {
                    // Add current tempNum to a variable
                    climberProp[propCount] = stod(tempNum);
                    propCount += 1;
                    // Reset tempNum
                    tempNum = "";
                }
            }
        }
    }

    int yLimit = wall.grid.size();
    int xLimit = wall.grid[0].size();
    array<int, 2> leftHand = {xLimit, yLimit};
    array<int, 2> rightHand = {xLimit, yLimit};
    array<int, 2> leftFoot= {xLimit, yLimit};
    array<int, 2> rightFoot = {xLimit, yLimit};
    for (int j = 0; j < wall.grid.size(); j ++) {
        for (int i = 0; i < wall.grid[j].size(); i++) {
            // Establishing starting hand holds
            if (wall.grid[j][i].isStartHand) {
                if (i < leftHand[0]) { // If new start is lefter than current left, set current left
                    rightHand = leftHand; // Set right to left
                    leftHand = {i, j}; // Set new left
                }
                else { // If not lefter, set right
                    rightHand = {i, j};
                }
            }
            if (wall.grid[j][i].isStartFoot) {
                if (i < leftFoot[0]) { // If new start is lefter than current left, set current left
                    rightFoot = leftFoot; // Set right to left
                    leftFoot = {i, j}; // Set new left
                }
                else { // If not lefter, set right
                    rightFoot = {i, j};
                }
            }
        }
    }

    // Checking for match hand start
    if (leftHand[0] < xLimit && leftHand[1] < yLimit) { // One or two legitimate start holds
        if (!(rightHand[0] < xLimit && rightHand[1] < yLimit)) { // One legitimate start hold
            // Therefore, match start
            rightHand = leftHand;
        }
    }
    else {
        cout << "no legitimate starting hand holds for climber" << endl;
        noClimber = true;
    }
    // Checking for match feet start
    if (leftFoot[0] < xLimit && leftFoot[1] < yLimit) { // One or two legitimate start holds
        if (!(rightFoot[0] < xLimit && rightFoot[1] < yLimit)) { // One legitimate start hold
            // Therefore, match start
            rightFoot = leftFoot;
        }
    }
    else {
        cout << "no legitimate starting foot holds for climber" << endl;
        noClimber = true;
    }

    if (!noClimber) {
        return(Climber(wall, leftHand, rightHand, leftFoot, rightFoot, climberProp[0], climberProp[1], climberProp[2]));
    }
    else {
        return(Climber());
    }
}

// Function to check if a string starts with a prefix.
bool startsWith(string fullString, string prefix) {
    if (!fullString.empty() && !prefix.empty()) {
        if (fullString.compare(0, prefix.size(), prefix) == 0) {
            return(true);
        }
        else {
            return(false);
        }
    }
    else {
        return(false);
    }
}

// Function to check if a string ends with a suffix.
bool endsWith(string fullString, string suffix) {
    if (!fullString.empty() && !suffix.empty()) {
        if (fullString.compare(fullString.rfind(suffix[0]), suffix.size(), suffix) == 0) {
            return(true);
        }
        else {
            return(false);
        }
    }
    else {
        return(false);
    }
}
