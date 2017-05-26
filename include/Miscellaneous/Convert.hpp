/*
 * Convert.hpp
 *
 *  Created on: Apr 30, 2017
 *      Author: farabiahmed
 */

#ifndef MISCELLANEOUS_CONVERT_HPP_
#define MISCELLANEOUS_CONVERT_HPP_

#include <iostream>
#include <string>     	// std::string, std::stod
#include <sstream>		// Conversion between string and primitives
#include <typeinfo>
#include <algorithm>	// std::count
#include <vector>
#include "Miscellaneous/SmartVector.hpp"

using namespace std;
/*
 *
 */
class Convert {
  public:

	template<typename T>
	static string T_to_string (T const &val)
	{
		std::ostringstream ostr;
		ostr << val;

		return ostr.str();
	}

	template <typename T>
	static T string_to_T(string const &val)
	{
		std::istringstream istr(val);
		T returnVal;

		if (!(istr >> returnVal))
			exitWithError("Config File Parser: Not a valid " + (string)typeid(T).name() + " received!\n");

		return returnVal;
	}

	static vector<string> Parse_String(const string& val, const char& delimiter);

	static void exitWithError(const string &error)
	{
		cout << error;
		cin.ignore();
		cin.get();

		exit(EXIT_FAILURE);
	}
};

/*
Template Specialization Note:

From: KRAkatau, cplusplus.com, 2012

The reason that templates have to be defined in headers is that the compiler wouldn't know
for which template parameters to compile the code and which functions to instantiate.

However in case of full template specialization, this question is no longer there - and the function
is compiled as normal. So when you include your header in several files - the function is defined
in multiple object files, which leads to the error. (multiple definition, first defined here)

So to fix this, you need to move the definition of your specialization into a cpp file,
but you have to leave the declaration in the header file to instruct the compiler that there exists
an explicit specialization of that function somewhere else, so that it doesn't instantiate
the template function for SmartVector when it meets a need for it.
*/

// Parsing character separated vector elements.
// Ex:
// myvector		= 3,4
template <> SmartVector Convert::string_to_T <SmartVector> (string const &val);

// Parsing character separated multiple vectors
// Ex:
// LOCATION_OF_TERMINALS	= 0,3;1,3
template <> vector<SmartVector> Convert::string_to_T < vector<SmartVector> > (string const &val);

// Parsing character separated double values
// Ex:
// REWARDS	=	1.0,-1.0
template <> vector<double> Convert::string_to_T < vector<double> > (string const &val);

// Parsing character separated int values
// Ex:
// ACTIVATION_FUNCTIONS	=	2;4
template <> vector<int> Convert::string_to_T < vector<int> > (string const &val);

// Parsing character separated int values
// Ex:
// ACTIVATION_FUNCTIONS	=	2;4
template <> vector<unsigned> Convert::string_to_T < vector<unsigned> > (string const &val);

// Parsing character separated int-array values
// Ex:
// HIDDEN_UNITS	=	2;4;8;4,4
template <> vector<vector<int>> Convert::string_to_T < vector<vector<int>> > (string const &val);

// Parsing character separated int-array values
// Ex:
// HIDDEN_UNITS	=	2;4;8;4,4
template <> vector<vector<unsigned>> Convert::string_to_T < vector<vector<unsigned>> > (string const &val);


#endif /* MISCELLANEOUS_CONVERT_HPP_ */
