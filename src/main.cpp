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
#include "Representations/StateActionValue.hpp"

#include "Agents/Agent.hpp"
#include "Agents/QIteration.hpp"
#include "Agents/TrajectoryBasedValueIteration.hpp"


using namespace std;


void help_menu(void)
{
	cout << "Reinfocement Learning							"<<endl;
}

int main()
{
	// For Random Number Generator
	srand(time(0));

	// Menu for user
	help_menu();

	// Class Pointers
	Environment* environment;
	Representation* value;
	Agent* agent;
	/*
	// Select Application
	int key=1;
	switch (key) {
		case 0:
			// QIteration Example for GridWorld
			environment = new Gridworld();
			value = new StateActionValue();
			agent = new QIteration(environment, value);
			agent->Start_Execution();
			value->Print_Value();
			break;

		case 1:
			// Trajectory Based Value Iteration Example for GridWorld
			environment = new Gridworld();
			value = new StateActionValue();
			agent = new TrajectoryBasedValueIteration(environment, value);
			agent->Start_Execution();
			agent->Test();
			break;

		default:
			break;
	}
	*/

	delete agent;
	delete environment;
	delete value;

	return 0;
}
