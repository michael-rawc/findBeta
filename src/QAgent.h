#pragma once

#include <tuple>
#include <unordered_map>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include "Agent.h"

using namespace std;

struct key_hash : unary_function<tuple<State, Action>, size_t> {
    size_t operator()(const tuple<State, Action>& key) const {
        State state = get<0>(key);
        Action action = get<1>(key);
        int stateStored = state.leftHand[0] ^ state.leftHand[1] ^ state.rightHand[0] ^ state.rightHand[1]
            ^ state.leftFoot[0] ^ state.leftFoot[1] ^ state.rightFoot[0] ^ state.rightFoot[1];
        int actionStored = action.extremity ^ action.toLocation[0] ^ action.toLocation[1];
        return(stateStored ^ actionStored);
    }
};

// Class for the Q-Learning agent.
class QAgent {
    public:
        double alpha;
        double gamma;
        double epsilon;
        Agent agent;
        unordered_map<tuple<State, Action>, double, key_hash> qTable;

        // Constructor for Q-Learning agent.
        QAgent(Agent ag, double al, double ga, double ep) {
            this->agent = ag;
            this->alpha = al;
            this->gamma = ga;
            this->epsilon = ep;

            // Initialise Q table
            this->qTable = this->initialiseQ();
        }

        // Function to update a Q value in the Q table.
        void updateQ(State state, Action action, State nextState, vector<Action> nextActions, double reward) {
            // If state action pair is not in Q table, add it
            tuple<State, Action> key = make_tuple(state, action);
            if (this->qTable.count(key) == 0) {
                this->addToQ(state, action);
            }

            // Find best next action
            vector<double> nextQs;

            // If no available next actions, add default value of 0
            if (nextActions.size() == 0) {
                nextQs.push_back(0);
            }
            else {
                for (int i = 0; i < nextActions.size(); i++) {
                    // If state action pair is not in Q table, add it
                    tuple<State, Action> tempKey = make_tuple(nextState, nextActions[i]);
                    if (this->qTable.count(tempKey) == 0) {
                        this->addToQ(nextState, nextActions[i]);
                    }
                    nextQs.push_back(this->qTable[tempKey]);
                }
            }

            // Get the best Q value
            double bestNextQ = *max_element(nextQs.begin(), nextQs.end());

            // Update Q value in Q table
            double currentQ = this->qTable[key];
            this->qTable[key] = currentQ + this->alpha * (reward + this->gamma * bestNextQ - currentQ);
        }

        // Function to choose an action based on the epsilon-greedy policy.
        Action getEGreedyAction() {
            if ((double) rand() / RAND_MAX < this->epsilon) {
                return(this->getRandomAction());
            }
            else {
                return(this->getBestAction());
            }
        }

        // Function to choose the best action from the available actions.
        Action getBestAction() {
            vector<double> qValues;

            // For each action, get the Q value
            for (int i = 0; i < this->agent.currentPossibleActions.size(); i++) {
                tuple<State, Action> key = make_tuple(this->agent.currentState, this->agent.currentPossibleActions[i]);
                // If key is in qTable, use value
                if (this->qTable.count(key) == 1) {
                    // cout << "getBestAction: found key" << endl;
                    qValues.push_back(this->qTable[key]);
                }
                // Else, use default
                else {
                    // cout << "getBestAction: no found key" << endl;
                    qValues.push_back(-10000);
                }
            }
            // Get the index of the best Q value in qValues
            int bestQIndex = getMaxIndexOfQ(qValues);

            // This _should_ ensure it never loops while testing
            if (qValues[bestQIndex] == -10000) {
                return(this->getRandomAction());
            }

            // Return the best action using bestQIndex
            return(this->agent.currentPossibleActions[bestQIndex]);
        }

        // Function to return a random action.
        Action getRandomAction() {
            int randomInt = rand() % this->agent.currentPossibleActions.size();
            return(this->agent.currentPossibleActions[randomInt]);
        }
    private:

        // Function to get the maximum index which also picks randomly if multiple maxima.
        int getMaxIndexOfQ(vector<double> qValues) {
            vector<int> maxIndexes;
            vector<double> maxValues;

            // Checks for qValues being of poor sizing
            if (qValues.size() == 0) {
                cout << "getMaxIndexOfQ: qValues has size 0" << endl;
                return(0);
            }
            else if (qValues.size() == 1) {
                // cout << "getMaxIndexOfQ: qValues has size 1" << endl;
                return(0);
            }
            else {
                // Assign initial maximal value from first comparison
                if (qValues[1] > qValues[0]) {
                    maxValues.push_back(qValues[1]);
                    maxIndexes.push_back(1);
                }
                else {
                    maxValues.push_back(qValues[0]);
                    maxIndexes.push_back(0);
                }

                // Past the first comparison, find any new maxima
                for (int i = 1; i < qValues.size(); i++) {
                    // If new maximal, clear vectors and add new values
                    if (qValues[i] > maxValues[0]) {
                        maxValues.clear();
                        maxValues.push_back(qValues[i]);
                        maxIndexes.clear();
                        maxIndexes.push_back(i);
                    }
                    // If value is the same as current maxima, add to vector
                    else if (qValues[i] == maxValues[0]) {
                        maxValues.push_back(qValues[i]);
                        maxIndexes.push_back(i);
                    }
                }

                // Pick random index from maxIndexes
                int randomInt = rand() % maxIndexes.size();

                return(maxIndexes[randomInt]);
            }
        }

        // Function to intialise the Q table.
        unordered_map<tuple<State, Action>, double, key_hash> initialiseQ() {
            unordered_map<tuple<State, Action>, double, key_hash> tempQTable;

            // For each action, add to Q table with value 0
            for (int i = 0; i < this->agent.currentPossibleActions.size(); i++) {
                tuple<State, Action> key = make_tuple(this->agent.currentState, this->agent.currentPossibleActions[i]);
                tempQTable[key] = 0;
            }

            return(tempQTable);
        }

        // Function to add a new state action pair to the Q table.
        void addToQ(State state, Action action) {
            tuple<State, Action> key = make_tuple(state, action);
            this->qTable[key] = 0;
        }
};
