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
#include "Environments/Blocksworld.hpp"

#include "QIteration/CleaningRobotDeterministic.hpp"
#include "QIteration/GridWorld.hpp"

#include "ProbabilityDistributions/ProbabilityDistribution.hpp"
#include "ProbabilityDistributions/DiscreteDistribution.hpp"

#include "Miscellaneous/SmartVector.hpp"
#include "Miscellaneous/ConfigParser.hpp"

#include "Representations/Representation.hpp"
#include "Representations/StateActionValue.hpp"
#include <Representations/FunctionApproximatorBinaryBasis.hpp>
#include <Representations/FunctionApproximatorGaussianRadialBasis.hpp>

#include "Agents/Agent.hpp"
#include "Agents/QIteration.hpp"
#include "Agents/TrajectoryBasedValueIteration.hpp"


using namespace std;


void help_menu(void)
{
	cout << "***********************************************************************************"<<endl;
	cout << "Deep Reinfocement Learning	Framework Main Routine									"<<endl;
	cout << "***********************************************************************************"<<endl;
	cout << "1- Grid World with Qiteration														"<<endl;
	cout << "2- Grid World with Trajectory Based Q-Value Iteration and QTable Representation	"<<endl;
	cout << "3- Grid World with Trajectory Based Q-Value Iteration and Binary BF Representation	"<<endl;
	cout << "4- Grid World with Trajectory Based Q-Value Iteration and GRBF Representation		"<<endl;
	cout << "5- Blocks World with Qiteration													"<<endl;
	cout << "***********************************************************************************"<<endl;
	cout << "Enter your choice and press return: ";

}

int main()
{
	// For Random Number Generator
	srand(time(0));

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

	// Select Application
	switch (choice)
	{
		case 1:
		{
			// QIteration Example for GridWorld

			// Get parameters from file
			ConfigParser cfg = ConfigParser("config/config_gridworld.cfg");

			// Initialize Environment
			environment 	= new Gridworld(cfg);

			// Initialize Representation Function
			value 			= new StateActionValue(*environment,cfg);

			// Initialize Solver
			agent 			= new QIteration(environment, value, cfg);

		}break;
		case 2:
		{
			// Trajectory Based Value Iteration with QTable Representation Example for GridWorld

			// Get parameters from file
			ConfigParser cfg = ConfigParser("config/config_gridworld.cfg");

			// Initialize Environment
			environment 	= new Gridworld(cfg);

			// Initialize Representation Function
			value 			= new StateActionValue(*environment,cfg);

			// Initialize Solver
			agent 			= new TrajectoryBasedValueIteration(environment, value, cfg);

		}break;
		case 3:
		{
			// Trajectory Based Value Iteration with BinaryBF Representation Example for GridWorld

			// Get parameters from file
			ConfigParser cfg = ConfigParser("config/config_gridworld.cfg");

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

			// Get parameters from file
			ConfigParser cfg = ConfigParser("config/config_gridworld.cfg");

			// Initialize Environment
			environment 	= new Gridworld(cfg);

			// Initialize Representation Function
			value 			= new FunctionApproximatorGaussianRadialBasis(*environment,cfg);

			// Initialize Solver
			agent 			= new TrajectoryBasedValueIteration(environment, value, cfg);

		}break;
		case 5:
		{
			// Qiteration Example for BlocksWorld

			// Get parameters from file
			ConfigParser cfg = ConfigParser("config/config_blocksworld.cfg");

			// Initialize Environment
			environment 	= new Blocksworld(cfg);

			// Initialize Representation Function
			value 			= new StateActionValue(*environment,cfg);

			// Initialize Solver
			agent 			= new QIteration(environment, value, cfg);


		}break;
		default:
			cout<< "You have entered an invalid input."<<endl;
			return 0;
			break;
	}


	//Start Calculation
	agent->Start_Execution();

	//Show Q-Values
	value->Print_Value();

	//Show Policy
	environment->Display_Policy(*value);

	delete agent;
	delete environment;
	delete value;

	return 0;
}
