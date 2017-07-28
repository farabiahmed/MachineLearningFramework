//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================


#include <iostream>

#include "Environments/Environment.hpp"
#include "Environments/Gridworld.hpp"
#include "Environments/Blocksworld.hpp"

#include "Agents/Agent.hpp"
#include "Agents/QIteration.hpp"
#include "Agents/TrajectoryBasedValueIteration.hpp"

#include "Representations/Representation.hpp"
#include "Representations/RepresentationUDP.hpp"
#include <Representations/FunctionApproximatorBinaryBasis.hpp>
#include <Representations/FunctionApproximatorGaussianRadialBasis.hpp>
#include <Representations/FunctionApproximatorNeuralNetwork.hpp>
#include <Representations/TabularStateActionPair.hpp>

#include <Miscellaneous/UserControl.hpp>

#define __cplusplus 201103L

using namespace std;

string Get_TimeStamp(void)
{
	time_t now = time(NULL);
	tm * timeinfo = localtime(&now);
	char buffer[80];
	strftime(buffer, 80, "%Y%m%d_%H%M%S", timeinfo);
	return string(buffer);
}

void help_menu(void)
{
	cout << "***********************************************************************************"<<endl;
	cout << "Deep Reinfocement Learning Framework Main Routine									"<<endl;
	cout << "***********************************************************************************"<<endl;
	cout << "1- Grid World with Qiteration														"<<endl;
	cout << "2- Grid World with Trajectory Based Q-Value Iteration and QTable Representation	"<<endl;
	cout << "3- Grid World with Trajectory Based Q-Value Iteration and Binary BF Representation	"<<endl;
	cout << "4- Grid World with Trajectory Based Q-Value Iteration and GRBF Representation		"<<endl;
	cout << "5- Grid World with Q-Iteration and Gaussian Radial Basis Function Representation	"<<endl;
	cout << "6- Grid World with Q-Iteration and NeuralNetworks Representation					"<<endl;
	cout << "7- Grid World with Trajectory Based Q-Value Iteration and NeuralNetworks Rep.		"<<endl;
	cout << "8- Blocks World with Qiteration													"<<endl;
	cout << "9- Grid World with Qiteration and External UDP Representation						"<<endl;
	cout << "***********************************************************************************"<<endl;
	cout << "Enter your choice and press return: ";

}

int main(int argc, char* argv[])
{
	// For Random Number Generator
	srand(time(0));

	string configFile = "";

	string timeStamp = Get_TimeStamp();

	// Check the number of parameters
	if (argc >= 2)
	{
		configFile = string(argv[1]);
	}

	// Store the user input
	int choice=0;

	// Menu for user
	help_menu();

	// Get The User Input
	cin >> choice;

	// End of user input
	cout << "***********************************************************************************"<<endl;

	// Class Pointers
	Environment* environment;
	Representation* value;
	Agent* agent;

	Neuron::alpha = 0.0;
	Neuron::eta = 0.1;
	Neuron::activation_function = Neuron::ACTIVATION_FUNCTION_TANH;

	// User interrupt added for main thread, to cut the iterations
	// on the fly and yet to get the reports. 
	// If you write "stop" to console the agent will stop. 
	UserControl& userControl = UserControl::GetInstance();

	// Select Application
	switch (choice)
	{
		case 1:
		{
			// QIteration Example for GridWorld

			// Assign the default file if it is not assigned yet.
			if(configFile.empty()) configFile = "config/config_gridworld.cfg";

			// Get parameters from file
			ConfigParser cfg = ConfigParser(configFile);

			// Initialize Environment
			environment 	= new Gridworld(cfg);

			// Initialize Representation Function
			value 			= new TabularStateActionPair(*environment,cfg);

			// Initialize Solver
			agent 			= new QIteration(environment, value, cfg);

		}break;
		case 2:
		{
			// Trajectory Based Value Iteration with QTable Representation Example for GridWorld

			// Assign the default file if it is not assigned yet.
			if(configFile.empty()) configFile = "config/config_gridworld.cfg";

			// Get parameters from file
			ConfigParser cfg = ConfigParser(configFile);

			// Initialize Environment
			environment 	= new Gridworld(cfg);

			// Initialize Representation Function
			value 			= new TabularStateActionPair(*environment,cfg);

			// Initialize Solver
			agent 			= new TrajectoryBasedValueIteration(environment, value, cfg);

		}break;
		case 3:
		{
			// Trajectory Based Value Iteration with BinaryBF Representation Example for GridWorld

			// Assign the default file if it is not assigned yet.
			if(configFile.empty()) configFile = "config/config_gridworld.cfg";

			// Get parameters from file
			ConfigParser cfg = ConfigParser(configFile);

			// Initialize Environment
			environment 	= new Gridworld(cfg);

			// Initialize Representation Function
			value 			= new FunctionApproximatorBinaryBasis(*environment,cfg);

			// Initialize Solver
			agent 			= new TrajectoryBasedValueIteration(environment, value, cfg);

		}break;
		case 4:
		{
			// Trajectory Based Value Iteration with RBF Representation Example for GridWorld

			// Assign the default file if it is not assigned yet.
			if(configFile.empty()) configFile = "config/config_gridworld.cfg";

			// Get parameters from file
			ConfigParser cfg = ConfigParser(configFile);

			// Initialize Environment
			environment 	= new Gridworld(cfg);

			// Initialize Representation Function
			value 			= new FunctionApproximatorGaussianRadialBasis(*environment,cfg);

			// Initialize Solver
			agent 			= new TrajectoryBasedValueIteration(environment, value, cfg);

		}break;
		case 5:
		{
			// Q-Iteration with Gaussian Radial Basis Function Representation Example for GridWorld

			// Assign the default file if it is not assigned yet.
			if(configFile.empty()) configFile = "config/config_gridworld.cfg";

			// Get parameters from file
			ConfigParser cfg = ConfigParser(configFile);

			// Initialize Environment
			environment 	= new Gridworld(cfg);

			// Initialize Representation Function
			value 			= new FunctionApproximatorGaussianRadialBasis(*environment,cfg);

			// Initialize Solver
			agent 			= new QIteration(environment, value, cfg);

		}break;
		case 6:
		{
			// Q-Iteration with Neural Network Representation Example for GridWorld

			// Assign the default file if it is not assigned yet.
			if(configFile.empty()) configFile = "config/config_gridworld.cfg";

			// Get parameters from file
			ConfigParser cfg = ConfigParser(configFile);

			// Initialize Environment
			environment 	= new Gridworld(cfg);

			// Initialize Representation Function
			value 			= new FunctionApproximatorNeuralNetwork(*environment,cfg);

			// Initialize Solver
			agent 			= new QIteration(environment, value, cfg);

		}break;
		case 7:
		{
			// Trajectory Based Value Iteration with Neural Network Representation Example for GridWorld

			// Assign the default file if it is not assigned yet.
			if(configFile.empty()) configFile = "config/config_gridworld.cfg";

			// Get parameters from file
			ConfigParser cfg = ConfigParser(configFile);

			// Initialize Environment
			environment 	= new Gridworld(cfg);

			// Initialize Representation Function
			value 			= new FunctionApproximatorNeuralNetwork(*environment,cfg);

			// Initialize Solver
			agent 			= new TrajectoryBasedValueIteration(environment, value, cfg);

		}break;
		case 8:
		{
			// Qiteration Example for BlocksWorld

			// Assign the default file if it is not assigned yet.
			if(configFile.empty()) configFile = "config/config_blocksworld.cfg";

			// Get parameters from file
			ConfigParser cfg = ConfigParser(configFile);

			// Initialize Environment
			environment 	= new Blocksworld(cfg);

			// Initialize Representation Function
			value 			= new TabularStateActionPair(*environment,cfg);

			// Initialize Solver
			agent 			= new QIteration(environment, value, cfg);


		}break;
		case 9:
		{
			// QIteration Example for GridWorld

			// Assign the default file if it is not assigned yet.
			if(configFile.empty()) configFile = "config/config_gridworld.cfg";

			// Get parameters from file
			ConfigParser cfg = ConfigParser(configFile);

			// Initialize Environment
			environment 	= new Gridworld(cfg);

			// Initialize Representation Function
			value 			= new RepresentationUDP(*environment,cfg);

			// Initialize Solver
			agent 			= new QIteration(environment, value, cfg);

		}break;
		default:
			cout<<endl<< "You have entered an invalid input, terminating..." <<endl<<endl;
			return 0;
			break;
	}

	cout<<endl<<endl;

	//Start Calculation
	agent->Start_Execution();

	//Show Q-Values
	value->Print_Value();

	//Show Policy
	environment->Display_Policy(*value);

	//Get Report
	agent->Get_Report("log/"+timeStamp,"agentReport.csv");

	//Get Report
	value->Get_Report("log/"+timeStamp,"representationReport.csv");

	cout<<endl<<"Done."<<endl<<endl;
	delete agent;
	delete environment;
	delete value;

	return 0;
}

