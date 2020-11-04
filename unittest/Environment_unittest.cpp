/*
 * Environment_unittest.cpp
 *
 *  Created on: Apr 26, 2017
 *      Author: farabiahmed
 * 
 *  Example: ./bin/Environment_unittest --CONFIG config/config_deliverydrone1d_10_1agent.cfg 
 * 
 */

#include <iostream>
#include <Miscellaneous/SmartVector.hpp>
#include <Environments/DeliveryDrone.hpp>
#include <Environments/Environment.hpp>
#include <Environments/EnvironmentFactory.hpp>
#include <Miscellaneous/BashFormatter.hpp>
#include <Miscellaneous/ConfigParser.hpp>
#include <Miscellaneous/CommandLineParser.hpp>

using namespace std;

// Test Case Id: Since this program is a unit test to verify the
// functionality of the related class, there are lots of test cases.
unsigned test_id = 0;
int gNumberOfAgents = 1;

// All Possible States and Actions
vector<SmartVector> States;
vector<SmartVector> Actions;

void help_menu(void)
{
	cout << "Environment Unit Testing...							"<<endl;
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

	for (int agent = 0; agent < gNumberOfAgents; ++agent)
	{
		cin>>c;
		switch(c)
		{
		case 'W':
			cout << "Up" << endl;//key up
			Action.elements[agent] = 0;
			break;
		case 'S':
			cout << "Down" << endl;   // key down
			Action.elements[agent] = 2;
			break;
		case 'A':
			cout << "Left" << endl;  // key left
			Action.elements[agent] = 3;
			break;
		case 'D':
			cout << "Right" << endl;  // key right
			Action.elements[agent] = 1;
			break;
		case 'E':
			cout << "Wait" << endl;  // key wait
			Action.elements[agent] = 4;
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
	return true;
}

int main(int argc, char* argv[])
{
	// For Random Number Generator
	srand(time(0));

	// Menu for user
	help_menu();

	/* Print commandline parameters */
	cout<<endl<<"Command Line Variables"<<endl;
	CommandLineParser clp = CommandLineParser(argc,argv);
	clp.Print_Arguments();

	/* Print configuration parameters */
	// Get parameters from file
	// ConfigParser cfg("config/config_deliverydrone_10x10_1agent.cfg");
	ConfigParser cfg(clp.GetOption<string>("CONFIG"));
	cout<<endl<<"Default Config Variables"<<endl;
	cfg.Print_Arguments();


	gNumberOfAgents = cfg.GetValueOfKey<int>("NUMBER_OF_AGENTS",1);

	// Create Environment
	cout << "Target Environment: " << cfg.GetValueOfKey<string>("ENVIRONMENT") << endl;
	Environment *env = EnvironmentFactory::Create(cfg.GetValueOfKey<string>("ENVIRONMENT"), cfg); //new MultiDeliveryDrone(cfg);
	if(env==NULL)
		cout << "Environment cannot be found..." << endl;
		
	// Create an instances for NextState and Action
	SmartVector Action(gNumberOfAgents);
	vector<SmartVector> actions;
	SmartVector NextState;
	SmartVector CurrentState;
	double Reward = 0.0;

	//Actions = env->Get_Action_List(CurrentState);
	//States = env->Get_All_Possible_States();

	CurrentState = env->Get_Random_State();
	print_test_information("Initial Position Check");
	env->Display_State(CurrentState);

	cout<<endl<<"*******************************************************************************************************"<<endl<<endl<<endl;

	while(1)
	{
		test_id++;
		print_test_information("Enter Action for New Movement:");

		if(!get_action_from_user(Action)) goto program_end;

		NextState = env->Get_Next_State(CurrentState,Action);

		Reward += env->Get_Reward(CurrentState,Action,NextState);

		env->Display_State(NextState);

		CurrentState = NextState;

		cout << "Reward: " << Reward << endl;

		if(env->Check_Terminal_State(CurrentState))
			break;

		cout<<endl<<"*******************************************************************************************************"<<endl<<endl<<endl;
	}

program_end:
	// Delete Environment Pointer
	delete env;

	// Inform the user that the program is ended.
	cout<<endl<<"End Of The Test Program"<<endl;

	return 0;
}
