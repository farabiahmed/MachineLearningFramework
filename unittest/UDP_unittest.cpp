//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <pthread.h>
#include <iostream>
#include <Communication/UDP.hpp>


using namespace std;

// Test Case Id: Since this program is a unit test to verify the
// functionality of the related class, there are lots of test cases.
unsigned test_id = 0;

//
UDP udpsocket;

void help_menu(void)
{
	cout << "UDP Unit Test...							"<<endl;
}

void print_test_information (string s)
{
	cout << endl  << "Test #" << test_id << " " << s <<  endl;
}

void* Timer(void *ptr)
{
	while (1)
	{
		//cout<<"Thread Timer 1sec..."<<endl;
		string data = "transmitting";
		udpsocket.WriteSocket((unsigned char*)data.c_str(),data.size());
		usleep(100000);
	}

	return 0;
}

void* Receiver(void *ptr)
{
	while (1)
	{
		//cout<<"Thread Receiver 1sec..."<<endl;

		unsigned char rxBuffer[65535];
		int len = udpsocket.ReadSocket(rxBuffer,65535);
		cout<<"UDP Receiver: "<<len<<" Data:"<<rxBuffer<<endl;
		usleep(1000);
	}

	return 0;
}

int main()
{
	// For Random Number Generator
	srand(time(0));

	// Menu for user
	help_menu();

	pthread_t threadTimer, threadReceiver;

	char ip[] = "127.0.0.1";
	udpsocket.Listen(4000);
	//udpsocket.CreateBroadcastServer(5000);
	udpsocket.SetTargetIp(ip,5000);

	pthread_create(&threadTimer, NULL, Timer, NULL);
	pthread_create(&threadReceiver, NULL, Receiver, NULL);

	// Wait for the thread to finish, then exit
	pthread_join(threadTimer, NULL);

	cout<<endl<<"End Of Test Program"<<endl;

	return 0;
}
