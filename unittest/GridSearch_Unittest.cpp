/*
 * GridSearch_Unittest.cpp
 *
 *  Created on: Apr 1, 2018
 *      Author: Farabi.Tarhan
 */

#include <iostream>
#include <unordered_map>
#include <cstdlib> // system command
#include <iomanip>      // std::setprecision
#include "Miscellaneous/ConfigParser.hpp"
#include <cmath>
#include <string>     	// std::string, std::stod
#include <sstream>		// Conversion between string and primitives
#include <map>			// Pair of key-value

using namespace std;

// Prototypes


// Global Variables
string filePath;
vector<vector<string>> param_list;

// GridSearch Related Variables:
string fileName;


// General Use Variables:
string timestamp;

void help_menu(void)
{
	cout << "Grid Search Test...							"<<endl;
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
	// Menu for user
	help_menu();

	// Get time for reporting
	timestamp = Get_TimeStamp();

	// Report File Directory:
	filePath = "log/" + timestamp;

	// Read GridSearch Configurations from config file
	ConfigParser cfg 				= ConfigParser("config/gridsearch_test.cfg");
	map<string, string> contents 	= cfg.GetContents();

	for (auto it : contents)
	{
		cout << it.first << " : ";

		param_list.push_back(cfg.GetValueOfKey<vector<string>>(it.first));

		for(auto it_vec : param_list[param_list.size()-1])
		{
			cout << it_vec << " ";
		}

		cout  << endl;
	}

	cout << endl << "Done" << endl;

	return 0;
}


