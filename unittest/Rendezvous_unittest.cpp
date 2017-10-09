//============================================================================
// Name        : template_unittest.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <iostream>
#include <sstream>
#include <string>

#include "Miscellaneous/SmartVector.hpp"

#include "Environments/Environment.hpp"
#include "Environments/Gridworld.hpp"
#include "Environments/Blocksworld.hpp"
#include "Environments/Rendezvous.hpp"

#include "Agents/Agent.hpp"
#include "Agents/QIteration.hpp"
#include "Agents/TrajectoryBasedValueIteration.hpp"
#include "Agents/GreedyAgent.hpp"
#include "Agents/OnlineTrajectoryValueIteration.hpp"

#include "Representations/Representation.hpp"
#include "Representations/RepresentationUDP.hpp"
#include <Representations/FunctionApproximatorBinaryBasis.hpp>
#include <Representations/FunctionApproximatorGaussianRadialBasis.hpp>
#include <Representations/FunctionApproximatorNeuralNetwork.hpp>
#include <Representations/TabularStateActionPair.hpp>

using namespace std;

void help_menu(void)
{
	cout << endl << endl;
	cout << "QIteration Example for Rendezvous Problem...		"<<endl;
	cout << "---------------------------------------------------"<<endl;
	cout << endl << endl;
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
	// Menu for user
	help_menu();

	string configFile = "";

	string timeStamp = Get_TimeStamp();

	// Class Pointers
	Environment* environment;
	Representation* value;
	Agent* agent;

	Neuron::alpha = 0.0;
	Neuron::eta = 0.1;
	Neuron::activation_function = Neuron::ACTIVATION_FUNCTION_TANH;


	// Assign the default file if it is not assigned yet.
	if(configFile.empty()) configFile = "config/config_rendezvous_3x3_3agent.cfg";

	// Get parameters from file
	ConfigParser cfg = ConfigParser(configFile);
	cfg.AddContent(string("TIME_STAMP"),timeStamp);

	// Initialize Environment
	environment 	= new Rendezvous(cfg);

	// Initialize Representation Function
	//value 			= new TabularStateActionPair(*environment,cfg);
	//value 				= new FunctionApproximatorNeuralNetwork(*environment,cfg);
	value 			= new RepresentationUDP(*environment,cfg);

	// Initialize Solver
	//agent 			= new QIteration(environment, value, cfg);
	agent 				= new TrajectoryBasedValueIteration(environment, value, cfg);
	//agent 				= new OnlineTrajectoryValueIteration(environment, value, cfg);
	//agent				= new GreedyAgent(environment,cfg);

	//Start Calculation
	agent->Start_Execution();

	//Show Q-Values
	//value->Print_Value();

	//Show Policy
	//environment->Display_Policy(*value);

	//Get Report
	agent->Get_Report("log/"+timeStamp,"agentReport.csv");

	//Get Report
	//value->Get_Report("log/"+timeStamp,"representationReport.csv");

	cout<<"Done."<<endl;

	delete agent;
	delete environment;
	delete value;

	return 0;
}
