//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
// Date		   : May,31,2017
//============================================================================

#include <iostream>
#include <sstream>
#include <string>
#include <omp.h>

using namespace std;

void help_menu(void)
{
	cout << "Parallel Programming Test...							"<<endl;
}

int main()
{
	// Menu for user
	help_menu();
	/*
    int thread_number;
    #pragma omp parallel private(thread_number)
    {
        #pragma omp for schedule(static) nowait
        for (int i = 0; i < 100; i++) {
            thread_number = omp_get_thread_num();
            cout << "Thread " << thread_number << " says " << i << endl;
        }
    }
	*/

	#pragma omp parallel
	{
		cout<<"Hello World"<<endl;
	}

	cout<<"Done..."<<endl;

	return 0;
}
