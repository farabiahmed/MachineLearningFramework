//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================


#include <iostream>
#include <cstdlib>
#include <cstring>
#include <Miscellaneous/ConfigParser.hpp>
#include <Miscellaneous/UserControl.hpp>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

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

vector<string> param_names; 			/* Holds set name */
vector<vector<string>> param_list;		/* List of elements in each set */
vector<float> param_results; 			/* Holds result of each model */

string executable_name;
string gridsearch_type;
int total_numberof_models = 1;

/* Prototypes */
vector<string> Get_Model_withIndex(vector<vector<string>>& list, int index);
string add_escape(string s);
void read_shared_memory(char* buffer, size_t size);
void Print_Search_List(void);

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

	cout << endl;
	cout << "---------------------------------------------------"<<endl;
	cout << "Search Parameters									"<<endl;
	cout << "---------------------------------------------------"<<endl;


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
			param_names.push_back(it.first);
			cout << it.first << ": ";

			param_list.push_back(cfg.GetValueOfKey<vector<string>>(it.first));

			for(auto it_vec : param_list[param_list.size()-1])
			{
				cout << it_vec << ",";
			}

			total_numberof_models *= param_list[param_list.size()-1].size();

			cout  << endl;
		}
	}

	param_results = vector<float>(total_numberof_models);

	cout << endl;
	cout << "---------------------------------------------------"<<endl;
	cout << "Miscellaneous Parameters							"<<endl;
	cout << "---------------------------------------------------"<<endl;

	cout << "Executable Name: " << executable_name << endl;
	cout << "Grid Search Type: " << gridsearch_type << endl;
	cout << "Total Number of Models: " << total_numberof_models << endl;

	Print_Search_List();

	cout << endl;
	cout << "---------------------------------------------------"<<endl;
	cout << "Execute Search List								"<<endl;
	cout << "---------------------------------------------------"<<endl;
	for (int i = 0; i < total_numberof_models; ++i) {
		/* Current model to run */
		vector<string> param =  Get_Model_withIndex(param_list, i);

		string arguments = "";
		for(unsigned j = 0; j < param.size(); ++j)
		{
			arguments += " -" + param_names[j] + " " + param[j];
		}

		arguments += " -TIME_STAMP " + timeStamp;
		arguments += " -MODEL_ID " + std::to_string(i);

		cout<<"Executing ";
		std::cout.width(3); cout << i;
		cout<< ". Model: (" << (executable_name + arguments) << ")" <<endl;

		string command = (executable_name + arguments);
		command = add_escape(command);
		system(command.c_str());

		char buffer[1024];
		float fval;
		read_shared_memory(buffer, sizeof buffer);
		sscanf(buffer,"%f",&fval);

		param_results[i] = fval;

		cout<<"Return Value:"<<fval<<endl;
		cout<<endl;
	}

	Print_Search_List();

	return 0;
}

vector<string> Get_Model_withIndex(vector<vector<string>>& list, int index)
{
	/* This params variable will hold selected elements of each set. */
	vector<string> params;

	/*
	 * Assume a search set which compromises 3 different set.
	 * Each set has different number of elements such as 3,2,3.
	 * Repetition Cycle would be 6,3,1.
	 * It means set1 will repeat every 6 cycle.
	 * And set2 will repeat every 3 cycle and so on.
	 */
	int repetition_cycle = 0;

	int total_elements_of_previoussets = 1;


	for (unsigned i = 0; i < list.size(); ++i) {

		total_elements_of_previoussets *= list[i].size();

		repetition_cycle = total_numberof_models / total_elements_of_previoussets;

		int setIndex = (index / repetition_cycle) % list[i].size();

		params.push_back(list[i][setIndex]);
	}

	return params;
}

string add_escape(string s)
{
	string str;

	for (unsigned i = 0; i < s.size(); ++i) {
		if(s.at(i) == ';')
			str.append(1u,'\\');
	    str.append(1u,s.at(i));
	}

	return str;
}

void read_shared_memory(char* buffer, size_t size)
{
    // ftok to generate unique key
	key_t key = ftok("mlfshmfile",65);

	// shmget returns an identifier in shmid
	int shmid = shmget(key,size,0666|IPC_CREAT);

	// shmat to attach to shared memory
	char *str  = (char*) shmat(shmid,(void*)0,0);

	memcpy(buffer,str, size);

	//detach from shared memory
	shmdt(str);

	// destroy the shared memory
	shmctl(shmid,IPC_RMID,NULL);
}

void Print_Search_List(void)
{
	cout << endl;
	cout << "---------------------------------------------------"<<endl;
	cout << "Search List										"<<endl;
	cout << "---------------------------------------------------"<<endl;

	cout << "Index,";
	for(auto it_vec : param_names)
	{
		cout << it_vec << ",";
	}
	cout << "Result";
	cout<<endl;

	for (int i = 0; i < total_numberof_models; ++i) {
		/* Current model to run */
		vector<string> param =  Get_Model_withIndex(param_list, i);

		std::cout.width(4); cout<< i; cout << ",";
		for(auto it_vec : param)
		{
			cout << it_vec << ",";
		}
		cout << to_string(param_results[i]);
		cout<< endl;
	}
}
