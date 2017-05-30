//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <pthread.h>
#include <iostream>
#include <unistd.h>




using namespace std;

// Test Case Id: Since this program is a unit test to verify the
// functionality of the related class, there are lots of test cases.
unsigned test_id = 0;

void help_menu(void)
{
	cout << "Thread Unit Test...							"<<endl;
}

void print_test_information (string s)
{
	cout << endl  << "Test #" << test_id << " " << s <<  endl;
}


void* Timer(void *ptr)
{
	while (1)
	{
		cout<<"Thread Working..."<<endl;

		usleep(100000);
	}

	return 0;
}


int main()
{
	// For Random Number Generator
	srand(time(0));

	// Menu for user
	help_menu();

	pthread_t threadTimer;

	pthread_create(&threadTimer, NULL, Timer, NULL);

	// Wait for the thread to finish, then exit
	pthread_join(threadTimer, NULL);

	cout<<endl<<"End Of Test Program"<<endl;

	return 0;
}
