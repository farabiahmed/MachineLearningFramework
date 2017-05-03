//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <iostream>
#include <unordered_map>

using namespace std;

void help_menu(void)
{
	cout << "Hastable Test...							"<<endl;
}

int main()
{
	// Menu for user
	help_menu();

	unordered_map<string, string> hashtable;
	hashtable.emplace("www.element14.com", "184.51.49.225");
	cout << "IP Address: " << hashtable["www.element14.com"] << endl;

	return 0;
}
