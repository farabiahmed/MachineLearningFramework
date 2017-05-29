//============================================================================
// Name        : CommandLineParser_unittest.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
// Date		   : May,29,2017
//============================================================================

#include <iostream>
#include <sstream>
#include <string>
#include <Miscellaneous/CommandLineParser.hpp>

using namespace std;

void help_menu(void)
{
	cout << endl;
	cout << "Command Line Parser Test...							"<<endl;
	cout << "-------------------------------------------------------"<<endl;
	cout << "Example Usage:											"<<endl;
	cout << "./bin/CommandLineParser_unittest -f abc -w 1920 -h 1080 -t 5t" << endl;
	cout << "-------------------------------------------------------"<<endl;
}

int main(int argc, char* argv[])
{
	// Menu for user
	help_menu();

	CommandLineParser clp = CommandLineParser(argc,argv);

	clp.Print_Arguments();


	string s = "-w";
	if( clp.ChecOptionExists("-w") )
	{
		int i = clp.GetOption<int>(s);

		cout<<"-w parameter (int):" << i  <<endl;
	}
	else
		cout<<"-w parameter not found"<<endl;


	cout<<endl<< "Done..." <<endl;
	return 0;
}
