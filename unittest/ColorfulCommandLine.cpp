//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <iostream>
#include "Miscellaneous/BashFormatter.hpp"

using namespace std;

void help_menu(void)
{
	for (int var = 0; var < 100; ++var) {
		cout << Formatter::MAGENTA<<"Colorful Command Line Test...		"<< Formatter::RESET<<endl;
	}

	cout << "Normal Test...														   "<<endl;
}

int main()
{
	// Menu for user
	help_menu();

	return 0;
}
