//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <iostream>
#include <unordered_map>
#include <fstream>

#include "Environments/Environment.hpp"
#include "Environments/Gridworld.hpp"

#include "QIteration/CleaningRobotDeterministic.hpp"
#include "QIteration/GridWorld.hpp"

#include "ProbabilityDistributions/ProbabilityDistribution.hpp"
#include "ProbabilityDistributions/DiscreteDistribution.hpp"

#include "Miscellaneous/SmartVector.hpp"
#include "Miscellaneous/ConfigParser.hpp"

#include "Representations/Representation.hpp"
#include "Representations/TabularStateActionPair.hpp"

#include "Agents/Agent.hpp"
#include "Agents/QIteration.hpp"
#include "Agents/TrajectoryBasedValueIteration.hpp"


using namespace std;


void help_menu(void)
{
	cout << "Gridworld with Trajectory Based Value Iteration Unit Test Program							"<<endl;
}

int main()
{
	// For Random Number Generator
	srand(time(0));

	// Menu for user
	help_menu();

	// Get parameters from file
	ConfigParser cfg("config/config_gridworld.cfg");

	// Class Pointers
	Environment* environment 	= new Gridworld(cfg);
	Representation* value 		= new TabularStateActionPair(*environment,cfg);
	Agent* agent 				= new TrajectoryBasedValueIteration(environment, value, cfg);

	// Start Calculation
	agent->Start_Execution();

	// Show Q-Values
	//value->Print_Value();
	agent->Test();


	delete agent;
	delete environment;
	delete value;

	return 0;
}
