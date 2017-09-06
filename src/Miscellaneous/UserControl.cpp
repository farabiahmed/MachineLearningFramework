/*
 * UserControl.cpp
 *
 *  Created on: Jun 1, 2017
 *      Author: farabiahmed
 */

#include <Miscellaneous/UserControl.hpp>

int UserControl::numberofcreationattempt = 0;
string UserControl::message = "";
pthread_mutex_t UserControl::lock_message = PTHREAD_MUTEX_INITIALIZER;

UserControl::UserControl()
{
	// Assign threads' tasks.
	pthread_create(&_thread, NULL, Thread_Distributer, NULL);

	if (pthread_mutex_init(&lock_message, NULL) != 0)
	{
		cout<<endl<<"ERROR:"<<"Mutex init failed"<<endl<<endl;
	}
}

UserControl::~UserControl()
{
	//cout<<"lock released"<<endl;
	pthread_mutex_destroy(&lock_message);
}

string UserControl::GetMessage(void)
{
	string ret;

	// Protect shared objects with mutex
	pthread_mutex_lock(&lock_message);
	ret = message;
	message = "";
	pthread_mutex_unlock(&lock_message);

	return ret;
}

void UserControl::GetStatus(void)
{
	cout<<endl;
	cout<<"User Control Status:"<<endl;
	cout<<"--------------------"<<endl;
	cout<<"Creation Attempt: "<< numberofcreationattempt <<endl;
}


void UserControl::Task_User_Interaction()
{
	cout<<endl<<"User Interaction Thread Started..."<<endl;
	bool threadStop = false;

	while (!threadStop)
	{
		string inputStr;

		// Get The User Input
		cin >> inputStr;

		// Inform User
		cout<<endl<<"User Input Received: "<< inputStr<<endl;

		// Convert all characters to lower-case
		std::transform(inputStr.begin(), inputStr.end(), inputStr.begin(), ::tolower);

		// Protect shared objects with mutex
		pthread_mutex_lock(&lock_message);
		message = inputStr;
		pthread_mutex_unlock(&lock_message);

		// Convert input to command object
		if (inputStr == "stop")
		{

		}
		else if (inputStr == "quit")
		{
			threadStop = true;
		}
	}
}

// Will not return until the internal thread has exited.
void UserControl::WaitForThreadToExit()
{
	(void) pthread_join(_thread, NULL);
}

