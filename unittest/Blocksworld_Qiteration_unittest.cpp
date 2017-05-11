/*
 * Blocksworld_Qiteration_unittest.cpp
 *
 *  Created on: May 2, 2017
 *      Author: farabiahmed
 */

#include <iostream>
#include <unordered_map>
#include <fstream>

#include "Environments/Environment.hpp"
#include "Environments/Blocksworld.hpp"

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
	cout << "Blocksworld with QIteration Unit Test Program							"<<endl;
}

int main()
{
	// For Random Number Generator
	srand(time(0));

	// Menu for user
	help_menu();

	// Get parameters from file
	ConfigParser cfg("config/config_blocksworld.cfg");

	// Class Pointers
	Environment* environment 	= new Blocksworld(cfg);
	Representation* value 		= new TabularStateActionPair(*environment,cfg);
	Agent* agent 				= new QIteration(environment, value, cfg);

	//Start Calculation
	agent->Start_Execution();

	//Show Q-Values
	//value->Print_Value();

	//Show Policy
	environment->Display_Policy(*value);

	delete agent;
	delete environment;
	delete value;

	return 0;
}
