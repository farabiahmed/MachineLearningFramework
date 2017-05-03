/*
 * Blocksworld_unittest.cpp
 *
 *  Created on: Apr 26, 2017
 *      Author: farabiahmed
 */

#include <iostream>
#include <Miscellaneous/SmartVector.hpp>
#include <Environments/Blocksworld.hpp>
#include <Environments/Environment.hpp>
#include "Miscellaneous/BashFormatter.hpp"
#include "Miscellaneous/ConfigParser.hpp"

using namespace std;

// Test Case Id: Since this program is a unit test to verify the
// functionality of the related class, there are lots of test cases.
unsigned test_id = 0;

// All Possible States and Actions
vector<SmartVector> States;
vector<SmartVector> Actions;

// Total Size Of Blocks Read From Config File.
int size_of_blocks;


void help_menu(void)
{
	cout << "Blocksworld Test...							"<<endl;
}

void print_test_information (string s)
{
	cout << endl << Formatter::BLUE << "Test #" << test_id << " " << s << Formatter::RESET << endl;
}

SmartVector generate_random_state()
{
	double random = rand()/(RAND_MAX+1.0);

	int random_state = random * States.size();

	return States[random_state];
}

SmartVector generate_random_action(const Environment& env, const SmartVector& state)
{
	Actions = env.Get_Action_List(state);

	double random = rand()/(RAND_MAX+1.0);

	int random_action = random * Actions.size();

	return Actions[random_action];
}

int main()
{
	// For Random Number Generator
	srand(time(0));

	// Menu for user
	help_menu();

	// Get parameters from file
	ConfigParser cfg("config/config_blocksworld.cfg");

	// Get the number of blocks.
	size_of_blocks = cfg.GetValueOfKey<int>("NUMBER_OF_BLOCKS");

	// Create Environment
	Environment *env = new Blocksworld(cfg);

	// Create an instances for NextState and Action
	SmartVector Action(2);
	vector<SmartVector> actions;
	SmartVector NextState;
	SmartVector CurrentState;

	States = env->Get_All_Possible_States();

	/*******************************************************************************************************/
	/* Check whether actions are reasonable and manipulating the state as expected.						   */
	/*******************************************************************************************************/
	test_id++;
	print_test_information("Check whether actions are reasonable and manipulating the state as expected:");

	// Set a random state
	CurrentState = generate_random_state();

	// Display State
	env->Display_State(CurrentState);

	for (int var = 0; var < 4; ++var) {

			// Configure an Action
			Action = generate_random_action(*env,CurrentState);

			// Display Action
			cout<<endl<<"Applying Action: ";
			env->Display_Action(Action);

			// Get Next State by applying that action to current state
			NextState = env->Get_Next_State(CurrentState,Action);

			// Display New State
			env->Display_State(NextState);

			// Get the new state as input for next experiment
			CurrentState = NextState;
	}


	/*******************************************************************************************************/
	/* Verify terminal state														 					   */
	/*******************************************************************************************************/
	test_id++;
	print_test_information("Verify terminal state:");

	// Set a random state
	CurrentState = generate_random_state();

	// Display State
	env->Display_State(CurrentState);

	cout<<endl<< "Is this state terminal?: " << (env->Check_Terminal_State(CurrentState)?"Yes":"No") << endl;

	/*******************************************************************************************************/
	/* Verify terminal state														 					   */
	/*******************************************************************************************************/
	test_id++;
	print_test_information("Verify terminal state:");

	// Set a random state
	CurrentState = generate_random_state();

	// Display State
	env->Display_State(CurrentState);

	cout<<endl<< "Is this state terminal?: " << (env->Check_Terminal_State(CurrentState)?"Yes":"No") << endl;

	/*******************************************************************************************************/
	/* Verify Non-Feasible/Blocked State														 		   */
	/*******************************************************************************************************/
	test_id++;
	print_test_information("Verify Non-Feasible/Blocked State:");

	// Set a random state
	CurrentState = generate_random_state();

	// Display State
	env->Display_State(CurrentState);

	cout<<endl<< "Is this state feasible?: " << (env->Check_Blocked_State(CurrentState)?"No":"Yes") << endl;


	/*******************************************************************************************************/
	/* Verify Non-Feasible/Blocked State														 		   */
	/*******************************************************************************************************/
	test_id++;
	print_test_information("Verify Non-Feasible/Blocked State:");

	// Set a random state
	CurrentState = generate_random_state();

	// Display State
	env->Display_State(CurrentState);

	cout<<endl<< "Is this state feasible?: " << (env->Check_Blocked_State(CurrentState)?"No":"Yes") << endl;

	/*******************************************************************************************************/
	/* Verifying All Possible States															 		   */
	/*******************************************************************************************************/
	test_id++;
	print_test_information("Verifying All Possible States:");

	vector<SmartVector> States = env->Get_All_Possible_States();

	for (unsigned i = 0; i < States.size(); ++i) {
		env->Display_State(States[i]);
	}

	/*******************************************************************************************************/

	// Delete Environment Pointer
	delete env;

	// Inform the user that the program is ended.
	cout<<endl<<"End Of The Test Program"<<endl;

	return 0;
}
