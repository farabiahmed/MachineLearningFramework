/*
 * UserControl.hpp
 *
 *  Created on: Jun 1, 2017
 *      Author: farabiahmed
 */

#ifndef MISCELLANEOUS_USERCONTROL_HPP_
#define MISCELLANEOUS_USERCONTROL_HPP_

/*
 *	User Control Class
 *	Use Cases:
 *		- To stop a loop without preventing other
 *		tasks with string based inputs.
 *
 *	How it gets input from user?
 *		- It listens user inputs via
 *			- Terminal (cin)
 *
 *	Design Pattern: Singleton
 *		- Since it is needed to created only one
 *		times.
 *		- It will give compile time error if
 *		one tries to create more than one instance
 *
 */
#include <iostream>
#include <pthread.h>
#include <string>
#include <algorithm>

using namespace std;

class UserControl {
public:

	static UserControl& GetInstance()
	{
		numberofcreationattempt++;

		static UserControl obj;
		return obj;
	}

	void GetStatus(void);

	string GetMessage(void);

	void WaitForThreadToExit();

private:
	UserControl();										// Private Constructor
	~UserControl();

	UserControl& operator=(const UserControl& copy) ; 	// Prevent Assignment

	UserControl(const UserControl& copy);				// Prevent Copy-Constructor

	static void* Thread_Distributer(void *arg)
	{
		UserControl* uc = reinterpret_cast<UserControl*>(arg);
		uc->Task_User_Interaction();

		delete uc;
		return 0;
	}

	void Task_User_Interaction();

	// Create thread.
	pthread_t _thread;

	// Last Message to check
	static string message;

	static int numberofcreationattempt;

	static pthread_mutex_t lock_message;
};



#endif /* MISCELLANEOUS_USERCONTROL_HPP_ */
