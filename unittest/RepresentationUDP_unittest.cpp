//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <iostream>
#include "Representations/Representation.hpp"
#include "Representations/RepresentationUDP.hpp"
#include "Representations/TabularStateActionPair.hpp"
#include "Environments/Environment.hpp"
#include "Environments/Gridworld.hpp"


using namespace std;

// Test Case Id: Since this program is a unit test to verify the
// functionality of the related class, there are lots of test cases.
unsigned test_id = 0;

void help_menu(void)
{
	cout << "UDP Unit Test...							"<<endl;
}

void print_test_information (string s)
{
	cout << endl  << "Test #" << test_id << " " << s <<  endl;
}

int main()
{
	// For Random Number Generator
	srand(time(0));

	// Menu for user
	help_menu();

	// Get parameters from file
	ConfigParser cfg = ConfigParser("config/config_gridworld.cfg");

	// Initialize Environment
	Environment *env 	= new Gridworld(cfg);
	Representation *rep = new RepresentationUDP(*env,cfg);

	SmartVector s = env->Get_All_Possible_States()[11];

	SmartVector a(1);


	a = env->Get_Action_List(s)[0];
	rep->Set_Value(s,a,0.54);
	a.index=0;

	a = env->Get_Action_List(s)[1];
	rep->Set_Value(s,a,0.99);


	a = env->Get_Action_List(s)[2];
	rep->Set_Value(s,a,-0.66);


	a = env->Get_Action_List(s)[3];
	rep->Set_Value(s,a,0.14);

	double val = rep->Get_Value(s,a);
	cout<<"Get Value: "<<val<<endl;

	auto p = rep->Get_Greedy_Pair(s);
	cout<<"Get Greedy Pair Arg: "<< p.first << " Val: "<< p.second<<endl;


	//rep->Get_Policy(s);
	cout<<endl<<"End Of Test Program"<<endl;

	return 0;
}
