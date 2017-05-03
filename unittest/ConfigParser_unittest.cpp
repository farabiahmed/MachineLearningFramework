//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <iostream>
#include <unordered_map>
#include "Miscellaneous/ConfigParser.hpp"
#include "Miscellaneous/SmartVector.hpp"


using namespace std;

void help_menu(void)
{
	cout << "ConfigParser Test...							"<<endl;
}

int main()
{
	// Menu for user
	help_menu();

    //ConfigParser cfg("/home/farabiahmed/Documents/workspace/Qiteration/Debug/config.cfg");

	ConfigParser cfg("../config/config.cfg");

	bool exists = cfg.KeyExists("car");
	std::cout << "car key: " << std::boolalpha << exists << "\n";
	exists = cfg.KeyExists("fruits");
	std::cout << "fruits key: " << exists << "\n";


	string someValue = cfg.GetValueOfKey<std::string>("mykey", "Unknown");
	cout << "value of key mykey: " << someValue << "\n";

	string carValue = cfg.GetValueOfKey<std::string>("car");
	cout << "value of key car: " << carValue << "\n";


	double doubleVal = cfg.GetValueOfKey<double>("double",99);
	cout << "value of key double: " << doubleVal << "\n\n";

	SmartVector vec = cfg.GetValueOfKey<SmartVector>("myvector");
	cout<<endl<<endl<<"myvector:"<<endl;
	vec.Print();


	vector<SmartVector> list = cfg.GetValueOfKey< vector<SmartVector> >("myset");
	cout<<endl<<endl<<"myset:"<<endl<<endl;
	for (unsigned i = 0; i < list.size(); ++i) {
		list[i].Print();
	}


	return 0;
}
