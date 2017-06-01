//============================================================================
// Name        : UserControl_unittest.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <iostream>
#include <sstream>
#include <string>
#include "Miscellaneous/UserControl.hpp"
#include <pthread.h>
#include <unistd.h>

using namespace std;

void help_menu(void)
{
	cout << endl;
	cout << "User Control Unit Test...							"<<endl;
	cout << "---------------------------------------------------"<<endl;
	cout << "Enter \"quit\" to terminate the threads.			"<<endl;
	cout << endl;
}

int main()
{
	// Menu for user
	help_menu();

	UserControl& userControl = UserControl::GetInstance();
	sleep(1);

	userControl.GetStatus();
	sleep(1);

	UserControl& userControl2 = UserControl::GetInstance();
	sleep(1);

	userControl2.GetStatus();
	sleep(1);

	userControl.WaitForThreadToExit();
	userControl2.WaitForThreadToExit();

	cout<<"Done."<<endl;

	return 0;
}
