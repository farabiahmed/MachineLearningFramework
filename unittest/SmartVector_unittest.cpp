//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <iostream>
#include <Miscellaneous/SmartVector.hpp>


using namespace std;

// Test Case Id: Since this program is a unit test to verify the
// functionality of the related class, there are lots of test cases.
unsigned test_id = 0;

void help_menu(void)
{
	cout << "SmartVector Test...							"<<endl;
}

void print_test_information (string s)
{
	cout << endl  << "Test #" << test_id << " " << s <<  endl;
}

int main()
{
	// For Random Number Generator
	srand(time(0));

	// Menu for user
	help_menu();


	/*******************************************************************************************************/
	/* Verifying Inner Product Functionality														 		   */
	/*******************************************************************************************************/
	test_id++;
	print_test_information("Verifying Inner Product Functionality:");

	SmartVector vec1(10);
	vec1.Initialize(2);

	SmartVector vec2(10);
	vec2.Initialize(-1.0);

	vec1.Print();

	vec2.Print();

	cout<<endl<<"Inner Product: "<<SmartVector::InnerProduct(vec1,vec2)<<endl;;



	/*******************************************************************************************************/
	/* Verifying Equality Comparison	 													 		   */
	/*******************************************************************************************************/
	test_id++;
	print_test_information("Verifying Equality Comparison:");

	SmartVector vec3(2);
	SmartVector vec4(2);
	vec3.Initialize();
	vec4.Initialize();

	vec3.Print();

	vec4.Print();

	if(vec3==vec4)
		cout<<endl<<"Comparison Result: Equal"<<endl;
	else
		cout<<endl<<"Comparison Result: Not Equal"<<endl;

	cout<<endl<<"End Of Test Program"<<endl;

	return 0;
}
