//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================


#include <iostream>
#include <cstdlib>
#include <Miscellaneous/ConfigParser.hpp>
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

vector<vector<string>> param_list;
string executable_name;
string gridsearch_type;
int total_numberof_models = 1;

void help_menu(void)
{
	cout << "***********************************************************************************"<<endl;
	cout << "Grid Search																		"<<endl;
	cout << "***********************************************************************************"<<endl;
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

	// Assign the default file if it is not assigned yet.
	if(configFile.empty()) configFile = "config/gridsearch.cfg";

	// Menu for user
	help_menu();

	// Get parameters from file
	ConfigParser cfg = ConfigParser(configFile);

	map<string, string> contents 	= cfg.GetContents();

	for (auto it : contents)
	{
		if( it.first == "EXECUTABLE")
		{
			executable_name = cfg.GetValueOfKey<string>(it.first);
		}
		else if( it.first == "GRIDSEARCH_TYPE")
		{
			gridsearch_type = cfg.GetValueOfKey<string>(it.first);
		}
		else
		{
			cout << it.first << " : \t";

			param_list.push_back(cfg.GetValueOfKey<vector<string>>(it.first));

			for(auto it_vec : param_list[param_list.size()-1])
			{
				cout << it_vec << " ";
			}

			total_numberof_models *= param_list[param_list.size()-1].size();

			cout  << endl;
		}
	}

	cout << "Executable Name: \t" << executable_name << endl;
	cout << "Grid Search Type: \t" << gridsearch_type << endl;
	cout << "Total Number of Models: \t" << total_numberof_models << endl;

	system((executable_name + " --config test123 --NUMBER_OF_AGENTS 5").c_str());

	return 0;
}

