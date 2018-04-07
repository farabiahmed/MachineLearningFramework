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

//	ConfigParser cfg("../config/config.cfg");
//	cout<<endl<<"Config Variables"<<endl;
//	cfg.Print_Arguments();

	cout<<endl<<"Command Line Variables"<<endl;
	CommandLineParser clp = CommandLineParser(argc,argv);
	clp.Print_Arguments();

	//cout<<endl<<"Updated Config Variables"<<endl;
	//cfg.Print_Arguments();


//	string timeStamp = Get_TimeStamp();
//
//
//	// Get parameters from file
//	ConfigParser cfg = ConfigParser(configFile);
//	cfg.AddContent(string("TIME_STAMP"),timeStamp);

	float f = 3.14;
	char array[1024];
	sprintf(array, "%f", f);
	write_shared_memory(array, sizeof array);

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
