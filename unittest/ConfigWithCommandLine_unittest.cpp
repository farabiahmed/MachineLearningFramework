//============================================================================
// Name        : ConfigWithCommandLine_unittest.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
// Date		   : March,26,2018
//============================================================================

#include <iostream>
#include <sstream>
#include <string>
#include <Miscellaneous/CommandLineParser.hpp>
#include <Miscellaneous/ConfigParser.hpp>

using namespace std;

void help_menu(void)
{
	cout << endl;
	cout << "Config With Command Line Test...						"<<endl;
	cout << "-------------------------------------------------------"<<endl;
	cout << "Example Usage:											"<<endl;
	cout << "./bin/ConfigWithCommandLine_unittest -color blue -double 1920" << endl;
	cout << "-------------------------------------------------------"<<endl;
}

int main(int argc, char* argv[])
{
	// Menu for user
	help_menu();

	ConfigParser cfg("../config/config.cfg");
	cout<<endl<<"Config Variables"<<endl;
	cfg.Print_Arguments();

	cout<<endl<<"Command Line Variables"<<endl;
	CommandLineParser clp = CommandLineParser(argc,argv);
	clp.Print_Arguments();

	cout<<endl<<"Updated Config Variables"<<endl;
	cfg.Print_Arguments();

	cout<<endl<< "Done..." <<endl;
	return 0;
}
