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
#include "Representations/RepresentationUDP.hpp"

#include "Agents/Agent.hpp"
#include "Agents/QIteration.hpp"
#include "Agents/TrajectoryBasedValueIteration.hpp"


using namespace std;


void help_menu(void)
{
	cout << "Gridworld with Trajectory Based Value Iteration Unit Test Program							"<<endl;
}

string Get_TimeStamp(void)
{
	time_t now = time(NULL);
	tm * timeinfo = localtime(&now);
	char buffer[80];
	strftime(buffer, 80, "%Y%m%d_%H%M%S", timeinfo);
	return string(buffer);
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
	//Representation* value 		= new TabularStateActionPair(*environment,cfg);
	//Representation* value 		= new FunctionApproximatorNeuralNetwork(*environment,cfg);
	Representation* value 		= new RepresentationUDP(*environment,cfg);
	Agent* agent 				= new TrajectoryBasedValueIteration(environment, value, cfg);

	// Start Calculation
	agent->Start_Execution();

	//Show Q-Values
	value->Print_Value();

	//Show Policy
	environment->Display_Policy(*value);

	//Get Report
	agent->Get_Report("log/"+Get_TimeStamp(),"agentReport.csv");

	//Get Report
	value->Get_Report("log/"+Get_TimeStamp(),"representationReport.csv");

	delete agent;
	delete environment;
	delete value;

	return 0;
}
