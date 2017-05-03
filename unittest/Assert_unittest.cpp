/*
 * Assert_unittest.cpp
 *
 *  Created on: Apr 29, 2017
 *      Author: farabiahmed
 */

#include <iostream>
#include <cassert>

using namespace std;


void help_menu(void)
{
	cout <<endl<< "Assert Unit Test Program							"<<endl;
}

int main()
{
	// Menu for user
	help_menu();

	int i=5;
	int compareValue = 5;

	assert(i<compareValue);

	cout<<endl << "End of Unit Test Program							"<<endl<<endl;

	return 0;
}
