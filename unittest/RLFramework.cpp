//============================================================================
// Name        : template_unittest.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <iostream>
#include <sstream>
#include <cstring>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <Miscellaneous/CommandLineParser.hpp>
#include <Miscellaneous/ConfigParser.hpp>
#include <Environments/EnvironmentFactory.hpp>
#include <Environments/Environment.hpp>
#include <Agents/Agent.hpp>
#include <Agents/AgentFactory.hpp>
#include <Representations/Representation.hpp>
#include <Representations/RepresentationFactory.hpp>

using namespace std;

void write_shared_memory(char* buffer, size_t size);

void help_menu(void)
{
	cout << endl;
	cout << "---------------------------------------------------"<<endl;
	cout << "RL Framework										"<<endl;
	cout << "---------------------------------------------------"<<endl;
}

string Get_TimeStamp(void)
{
	time_t now = time(NULL);
	tm * timeinfo = localtime(&now);
	char buffer[80];
	strftime(buffer, 80, "%Y%m%d_%H%M%S", timeinfo);
	return string(buffer);
}

string configFile;

int main(int argc, char* argv[])
{
	// Menu for user
	help_menu();

	/* Print commandline parameters */
	cout<<endl<<"Command Line Variables"<<endl;
	CommandLineParser clp = CommandLineParser(argc,argv);
	clp.Print_Arguments();

	/* Print configuration parameters */
	ConfigParser cfg(clp.GetOption<string>("CONFIG"));
	cout<<endl<<"Default Config Variables"<<endl;
	cfg.Print_Arguments();

	/* Update configuration via command line arguments */
	unordered_map<string, string> contents = clp.GetContents();
	for(auto it : contents)
	{
		cfg.UpdateContent(it.first,it.second);
	}

	/* Print updated configuration parameters */
	cout<<endl<<"Updated Config Variables"<<endl;
	cfg.Print_Arguments();

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

	/* Run Model Here */
	Environment *environment 	= EnvironmentFactory::Create(clp.GetOption<string>("ENVIRONMENT"),cfg);
	Representation *value		= RepresentationFactory::Create(clp.GetOption<string>("REPRESENTATION"), environment, cfg);
	Agent *agent 				= AgentFactory::Create(clp.GetOption<string>("AGENT"), environment, value, cfg);

	//environment->Display_State(environment->Get_Initial_State());

	//Start Calculation
	agent->Start_Execution();

	//Show Q-Values
	//value->Print_Value();

	//Show Policy
	//environment->Display_Policy(*value);

	//Get Report
	agent->Get_Report("log/" +
						cfg.GetValueOfKey<string>("TIME_STAMP","00000000_000000"),
						"agentReport_" +
						cfg.GetValueOfKey<string>("MODEL_ID", "0") +
						".csv");

	//Get Report
	//value->Get_Report("log/"+timeStamp,"representationReport.csv");


	/* Return Performance Value */
	float f = agent->Get_Score();
	char array[1024];
	sprintf(array, "%f", f);
	write_shared_memory(array, sizeof array);


	delete agent;
	delete environment;
	delete value;

	cout<<"Done."<<endl;

	return 0;
}

void write_shared_memory(char* buffer, size_t size)
{
    // ftok to generate unique key
    key_t key = ftok("mlfshmfile",65);

    // shmget returns an identifier in shmid
    int shmid = shmget(key,size,0666|IPC_CREAT);

    // shmat to attach to shared memory
    char *str = (char*) shmat(shmid,(void*)0,0);

    memcpy(str, buffer, size);

    //detach from shared memory
    shmdt(str);
}
