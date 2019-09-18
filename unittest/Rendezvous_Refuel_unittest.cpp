/*
 * Blocksworld_unittest.cpp
 *
 *  Created on: Apr 26, 2017
 *      Author: farabiahmed
 */

#include <iostream>
#include <Miscellaneous/SmartVector.hpp>
#include <Environments/Gridworld_Refuel.hpp>
#include <Environments/Rendezvous_Refuel.hpp>
#include <Environments/Environment.hpp>
#include "Miscellaneous/BashFormatter.hpp"
#include "Miscellaneous/ConfigParser.hpp"

using namespace std;

// Test Case Id: Since this program is a unit test to verify the
// functionality of the related class, there are lots of test cases.
unsigned test_id = 0;

int number_of_agents;

// All Possible States and Actions
vector<SmartVector> States;
vector<SmartVector> Actions;

void help_menu(void)
{
	cout << "Rendezvous Refuel Test...							"<<endl;
}

void print_test_information (string s)
{
	cout << endl << Formatter::GREEN << "Test #" << test_id << " " << s << Formatter::RESET << endl;
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

bool get_action_from_user(SmartVector &Action)
{
	char c;
	SmartVector temp(number_of_agents);

	for (int i = 0; i < number_of_agents; ++i)
	{
		cin>>c;
		switch(c)
		{
		case 'W':
			cout <<"#" << i << " Up" << endl;//key up
			temp.elements[i]=0;
			break;
		case 'S':
			cout <<"#" << i << " Down" << endl;   // key down
			temp.elements[i]=2;
			break;
		case 'A':
			cout <<"#" << i << " Left" << endl;  // key left
			temp.elements[i]=3;
			break;
		case 'D':
			cout <<"#" << i << " Right" << endl;  // key right
			temp.elements[i]=1;
			break;
		case 'E':
			cout <<"#" << i << " Wait" << endl;  // key wait
			temp.elements[i]=4;
			break;
		case 'Q':
			cout << endl << "Quit" << endl;  // key quit
			return false;
			break;
		default:
			cout << endl << "Invalid Input" << endl;  // not arrow
			return false;
			break;
		}

	}
	cout<<endl;
	Action = temp;
	return true;
}

int main()
{
	// For Random Number Generator
	srand(time(0));

	// Menu for user
	help_menu();

	// Get parameters from file
	ConfigParser cfg("config/config_rendezvous_refuel_5x5_3agent.cfg");
	number_of_agents = cfg.GetValueOfKey<int>("NUMBER_OF_AGENTS");

	// Create Environment
	Environment *env = new Rendezvous_Refuel(cfg);

	// Create an instances for NextState and Action
	SmartVector Action;
	vector<SmartVector> actions;
	SmartVector NextState;
	SmartVector CurrentState;

	Actions = env->Get_Action_List(CurrentState);
	States = env->Get_All_Possible_States();

	CurrentState = env->Get_Initial_State();
	print_test_information("Initial Position Check");
	env->Display_State(CurrentState);

	cout<<endl<<"*******************************************************************************************************"<<endl<<endl<<endl;

	while(1)
	{
		test_id++;
		print_test_information("Enter Action for New Movement:");

		if(!get_action_from_user(Action)) goto program_end;

		NextState = env->Get_Next_State(CurrentState,Action);

		env->Display_State(NextState);

		CurrentState = NextState;

		cout<<endl<<"*******************************************************************************************************"<<endl<<endl<<endl;
	}

//	/*******************************************************************************************************/
//	/* Check whether actions are reasonable and manipulating the state as expected.						   */
//	/*******************************************************************************************************/
//	test_id++;
//	print_test_information("Check whether actions are reasonable and manipulating the state as expected:");
//
//	// Set a random state
//	CurrentState = generate_random_state();
//
//	// Display State
//	env->Display_State(CurrentState);right
//
//	for (int var = 0; var < 4; ++var) {
//
//			// Configure an Action
//			Action = generate_random_action(*env,CurrentState);
//
//			// Display Action
//			cout<<endl<<"Applying Action: ";
//			env->Display_Action(Action);
//
//			// Get Next State by applying that action to current state
//			NextState = env->Get_Next_State(CurrentState,Action);
//
//			// Display New State
//			env->Display_State(NextState);
//
//			// Get the new state as input for next experiment
//			CurrentState = NextState;
//	}
//
//
//	/*******************************************************************************************************/
//	/* Verify terminal state														 					   */
//	/*******************************************************************************************************/
//	test_id++;
//	print_test_information("Verify terminal state:");
//
//	// Set a random state
//	CurrentState = generate_random_state();
//
//	// Display State
//	env->Display_State(CurrentState);
//
//	cout<<endl<< "Is this state terminal?: " << (env->Check_Terminal_State(CurrentState)?"Yes":"No") << endl;
//
//	/*******************************************************************************************************/
//	/* Verify terminal state														 					   */
//	/*******************************************************************************************************/
//	test_id++;
//	print_test_information("Verify terminal state:");
//
//	// Set a random state
//	CurrentState = generate_random_state();
//
//	// Display State
//	env->Display_State(CurrentState);
//
//	cout<<endl<< "Is this state terminal?: " << (env->Check_Terminal_State(CurrentState)?"Yes":"No") << endl;
//
//	/*******************************************************************************************************/
//	/* Verify Non-Feasible/Blocked State														 		   */
//	/*******************************************************************************************************/
//	test_id++;
//	print_test_information("Verify Non-Feasible/Blocked State:");
//
//	// Set a random state
//	CurrentState = generate_random_state();
//
//	// Display State
//	env->Display_State(CurrentState);
//
//	cout<<endl<< "Is this state feasible?: " << (env->Check_Blocked_State(CurrentState)?"No":"Yes") << endl;
//
//
//	/*******************************************************************************************************/
//	/* Verify Non-Feasible/Blocked State														 		   */
//	/*******************************************************************************************************/
//	test_id++;
//	print_test_information("Verify Non-Feasible/Blocked State:");
//
//	// Set a random state
//	CurrentState = generate_random_state();
//
//	// Display State
//	env->Display_State(CurrentState);
//
//	cout<<endl<< "Is this state feasible?: " << (env->Check_Blocked_State(CurrentState)?"No":"Yes") << endl;
//
//	/*******************************************************************************************************/
//	/* Verifying All Possible States															 		   */
//	/*******************************************************************************************************/
//	test_id++;
//	print_test_information("Verifying All Possible States:");
//
//	vector<SmartVector> States = env->Get_All_Possible_States();
//
//	for (unsigned i = 0; i < States.size(); ++i) {
//		env->Display_State(States[i]);
//	}
//
//	/*******************************************************************************************************/

program_end:
	// Delete Environment Pointer
	delete env;

	// Inform the user that the program is ended.
	cout<<endl<<"End Of The Test Program"<<endl;

	return 0;
}
