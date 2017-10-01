/*
 * ConfigParser.cpp
 *
 *  Created on: Mar 12, 2017
 *      Author: farabiahmed
 */

#include "Miscellaneous/ConfigParser.hpp"

ConfigParser::ConfigParser(const string &fname) {
	// TODO Auto-generated constructor stub
	this->FileName = fname;

	ExtractKeys();
}

ConfigParser::~ConfigParser() {
	// TODO Auto-generated destructor stub
}

bool ConfigParser::ExtractKeys(){

	// Open File
	fstream file(FileName.c_str(),file.in);

	// Check if file opened
	if(!file)
	{
		exitWithError("Config File Parser: Config file does not found.");
	}

	string line;
	size_t lineNo = 0;

	// Loop through file
	while(getline(file,line))
	{
		lineNo++;

		// If line is empty pass this iteration.
		if(line.empty()) continue;

		// Remove any comment from current line.
		RemoveComment(line);

		// If line is blank pass this iteration.
		if(CheckBlankLine(line)) continue;

		ParseLine(line, lineNo);
	}

	// Close file.
	file.close();

	return true;
}

void ConfigParser::RemoveComment(string &line) const
{
	if (line.find('#') != line.npos)
		line.erase(line.find('#'));
}

bool ConfigParser::CheckBlankLine(const std::string &line) const
{
	// find_first_not_of:
	// Finds the first character equal to none of the characters
	// in the given character sequence. The search considers only
	// the interval [pos, size()). If the character is not present
	// in the interval, npos will be returned.

	// true: if line is blank such as "" or "   ".
	// false: otherwise.
	return (line.find_first_not_of(' ') == line.npos);
}

void ConfigParser::ParseLine(const string &line, size_t const lineNo)
{
	if (line.find('=') == line.npos)
		exitWithError("Config File Parser: Couldn't find separator on line: " + Convert::T_to_string(lineNo) + "\n");

	if (!ValidLine(line))
		exitWithError("Config File Parser: Bad format for line: " + Convert::T_to_string(lineNo) + "\n");

	ExtractContents(line);
}

bool ConfigParser::ValidLine(const std::string &line) const
{
	std::string temp = line;
	temp.erase(0, temp.find_first_not_of("\t "));
	if (temp[0] == '=')
		return false;

	for (size_t i = temp.find('=') + 1; i < temp.length(); i++)
		if (temp[i] != ' ')
			return true;

	return false;
}

void ConfigParser::ExtractContents(const string &line)
{
	std::string temp = line;
	temp.erase(0, temp.find_first_not_of("\t "));
	size_t sepPos = temp.find('=');

	std::string key, value;
	ExtractKey(key, sepPos, temp);
	ExtractValue(value, sepPos, temp);

	AddContent(key,value);
}

void ConfigParser::AddContent(const std::string &key, const std::string &value)
{
	if (!KeyExists(key))
		contents.insert(std::pair<std::string, std::string>(key, value));
	else
		exitWithError("CFG: Can only have unique key names!\n");
}

void ConfigParser::ExtractKey(std::string &key, size_t const &sepPos, const std::string &line) const
{
	key = line.substr(0, sepPos);
	if (key.find('\t') != line.npos || key.find(' ') != line.npos)
		key.erase(key.find_first_of("\t "));
}

void ConfigParser::ExtractValue(std::string &value, size_t const &sepPos, const std::string &line) const
{
	value = line.substr(sepPos + 1);
	value.erase(0, value.find_first_not_of("\t "));
	value.erase(value.find_last_not_of("\t ") + 1);
}

bool ConfigParser::KeyExists(const std::string &key) const
{
	return contents.find(key) != contents.end();
}

void ConfigParser::exitWithError(const string &error)
{
	cout << error;
	cin.ignore();
	cin.get();

	exit(EXIT_FAILURE);
}
