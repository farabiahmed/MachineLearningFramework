//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <iostream>

using namespace std;

void help_menu(void)
{
	cout << "String Test...							"<<endl;
}

int main()
{
	// Menu for user
	help_menu();

    string line = "";
    cout<< line.find_first_not_of(' ') << endl;
    cout<< line.npos << endl;

    line = "aaabcdef";
	cout<< line.find_first_not_of('a') << endl;

	return 0;
}
