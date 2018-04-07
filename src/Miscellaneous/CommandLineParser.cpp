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
		string command;
		if(string(argv[i]).rfind('-',0) == 0) command = string(argv[i]).substr(1);
		if(string(argv[i]).rfind(string("--").c_str(),0) == 0) command = string(argv[i]).substr(2);
		dictionary.emplace(command,string(argv[i+1]));
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


