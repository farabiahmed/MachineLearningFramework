#include <iostream>
#include <string>     	// std::string, std::stod
#include <sstream>		// Conversion between string and primitives
#include <typeinfo>
#include <algorithm>	// std::count
#include <vector>
#include "Miscellaneous/BashFormatter.hpp"
#include "Miscellaneous/SmartVector.hpp"
#include "Miscellaneous/Convert.hpp"

using namespace std;



void help_menu(void)
{
	cout << "Blocksworld Test...							"<<endl;
}

void print_test_information (string s)
{
	// Test Case Id: Since this program is a unit test to verify the
	// functionality of the related class, there are lots of test cases.
	static unsigned test_id = 0;

	test_id++;

	cout << endl << Formatter::BLUE << "Test #" << test_id << " " << s << Formatter::RESET << endl;
}

int main () {

	int i=5;
	double d = 9.99;
	string s;

	/*******************************************************************************************************/
	/* Verifying integer to String conversion				   */
	/*******************************************************************************************************/
	print_test_information("Verifying integer to String conversion :");

	s = Convert::T_to_string<int>(i);
	cout<<endl<<s<<endl;


	/*******************************************************************************************************/
	/* Verifying String to Double conversion				   */
	/*******************************************************************************************************/
	print_test_information("Verifying String to Double conversion :");

	d = 65.51;
	s = "1989.87";
	d = Convert::string_to_T<double>(s);
	cout<<endl<<d<<endl;

	/*******************************************************************************************************/
	/* Template Specialization for SmartVector																	   */
	/*******************************************************************************************************/
	print_test_information("Template Specialization	for SmartVector :");

	s = "3.14,51.5";
	SmartVector vec = Convert::string_to_T<SmartVector>(s);

	vec.Print();

	/*******************************************************************************************************/
	/* Template Specialization for vector<SmartVector>																	   */
	/*******************************************************************************************************/
	print_test_information("Template Specialization for vector<SmartVector> :");

	s = "3.14,51.5;65.51,96.998;1.0009,2.00008";

	vector<SmartVector> list = Convert::string_to_T< vector<SmartVector>	>(s);

	for (unsigned i = 0; i < list.size(); ++i) {
		list[i].Print();
	}


	cout<<endl<<"End Of Unit Test"<<endl<<endl;

	return 0;
}
