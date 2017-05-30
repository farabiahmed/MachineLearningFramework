//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

void help_menu(void)
{
	cout << "String and Stringstream Test...							"<<endl;
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

	int i = 51;
	stringstream ss;
	ss<<i;
	string s = ss.str();

	cout<<"Integer Conversion Result: "<< s<<endl;


	s = std::to_string(i);
	cout<<"Integer Conversion Result: "<< s<<endl;

	return 0;
}
