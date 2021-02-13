/*
 * Environment Speed Test
 *
 * time ./bin/EnvironmentSpeed_Unittest --CONFIG config/config_multideliverydrone_5x5_3agent.cfg --REPRESENTATION RepresentationEmpty --MAX_NUMBER_OF_ITERATION 100
 * 20191030 20.5 secs
 *
 * Play 100 Game
 *
 *  Created on: Apr 26, 2017
 *      Author: farabiahmed
 */

#include <iostream>
#include <Miscellaneous/SmartVector.hpp>
#include <Environments/DeliveryDrone.hpp>
#include <Environments/EnvironmentFactory.hpp>
#include <Environments/Environment.hpp>
#include <Agents/Agent.hpp>
#include <Agents/AgentFactory.hpp>
#include <Representations/Representation.hpp>
#include <Representations/RepresentationFactory.hpp>
#include "Miscellaneous/BashFormatter.hpp"
#include "Miscellaneous/ConfigParser.hpp"
#include <Miscellaneous/CommandLineParser.hpp>

using namespace std;

// All Possible States and Actions
vector<SmartVector> States;
vector<SmartVector> Actions;

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

string Get_TimeStamp(void)
{
	time_t now = time(NULL);
	tm * timeinfo = localtime(&now);
	char buffer[80];
	strftime(buffer, 80, "%Y%m%d_%H%M%S", timeinfo);
	return string(buffer);
}

int main(int argc, char* argv[])
{
	// For Random Number Generator
	srand(time(0));

	CommandLineParser clp = CommandLineParser(argc,argv);
	clp.Print_Arguments();

	/* Print configuration parameters */
	ConfigParser cfg(clp.GetOption<string>("CONFIG"));
	cfg.Print_Arguments();

	/* Update configuration via command line arguments */
	unordered_map<string, string> contents = clp.GetContents();
	for(auto it : contents)
	{
		cfg.UpdateContent(it.first,it.second);
	}

	/* Add Time stamp if not exist */
	if(!cfg.KeyExists("TIME_STAMP"))
	{
		string timeStamp = Get_TimeStamp();
		cfg.AddContent(string("TIME_STAMP"),timeStamp);
	}

    /* Get Model Id */
	if(!cfg.KeyExists("MODEL_ID"))
	{
		cfg.AddContent(string("MODEL_ID"),string("0"));
	}

	// Create Environment
	/* Run Model Here */
	Environment *environment 	= EnvironmentFactory::Create(cfg.GetValueOfKey<string>("ENVIRONMENT"),cfg);
	Representation *value		= RepresentationFactory::Create(cfg.GetValueOfKey<string>("REPRESENTATION"), environment, cfg);
	Agent *agent 				= AgentFactory::Create(cfg.GetValueOfKey<string>("AGENT"), environment, value, cfg);

	// Create an instances for NextState and Action
	SmartVector action;
	SmartVector state;

	//Actions = env->Get_Action_List(CurrentState);
	//States = env->Get_All_Possible_States();

	state = environment->Get_Random_State();
	action  = generate_random_action(*environment, state);

	for(int game = 0; game <  cfg.GetValueOfKey<int>("MAX_NUMBER_OF_ITERATION",50); game++)
	{

		cout<<"Game #: " << setw(6) << game << flush;

		agent->Get_Cumulative_Rewards(game);
		cout << endl;
	}


	// Delete Environment Pointer
	delete environment; delete agent; delete value;

	// Inform the user that the program is ended.
	cout<<endl<<"End Of The Test Program"<<endl;

	return 0;
}
