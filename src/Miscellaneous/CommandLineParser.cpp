/*
 * CommandLineParser.cpp
 *
 *  Created on: May 29, 2017
 *      Author: farabiahmed
 */

#include <Miscellaneous/CommandLineParser.hpp>

CommandLineParser::CommandLineParser(int argc, char* argv[]) {

	for (int i = 1; i < argc; ++i) {
		//cout<< "Argument #"<<i<<" "<<argv[i]<<endl;
		dictionary.emplace(string(argv[i]),string(argv[i+1]));
		i++;
	}


}

CommandLineParser::~CommandLineParser() {
	// TODO Auto-generated destructor stub
}

bool CommandLineParser::ChecOptionExists(const string&  param)
{
	auto got = dictionary.find(param);

	if(got == dictionary.end())
		return false;
	else
		return true;
}

void CommandLineParser::Print_Arguments(void)
{
	for (auto it : dictionary)
	{
		cout << it.first << " : " << it.second << endl;
	}
}


