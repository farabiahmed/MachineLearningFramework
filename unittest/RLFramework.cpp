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
#include <Miscellaneous/CommandLineParser.hpp>
using namespace std;

void help_menu(void)
{
	cout << endl;
	cout << "---------------------------------------------------"<<endl;
	cout << "RL Framework for Different Simulations				"<<endl;
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

	cout<<"Done."<<endl;

	return 0;
}
