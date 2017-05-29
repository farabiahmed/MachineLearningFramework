/*
 * CommandLineParser.hpp
 *
 *  Created on: May 29, 2017
 *      Author: farabiahmed
 */

#ifndef MISCELLANEOUS_COMMANDLINEPARSER_HPP_
#define MISCELLANEOUS_COMMANDLINEPARSER_HPP_



#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "Miscellaneous/Convert.hpp"

using namespace std;
/*
 *
 */
class CommandLineParser {
public:
	CommandLineParser(int argc, char* argv[]);
	virtual ~CommandLineParser();
	bool ChecOptionExists(const string&  param);

	template <typename ValueType>
	ValueType GetOption(const string&  param)
	{
		return Convert::string_to_T<ValueType>(dictionary[param]);
	}

	void Print_Arguments(void);


private:
	vector<string> words;

	unordered_map<string, string> dictionary;

};

#endif /* MISCELLANEOUS_COMMANDLINEPARSER_HPP_ */
