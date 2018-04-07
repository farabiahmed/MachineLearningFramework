/*
 * ConfigParser.hpp
 *
 *  Created on: Mar 12, 2017
 *      Author: farabiahmed
 */

#ifndef MISCELLANEOUS_CONFIGPARSER_HPP_
#define MISCELLANEOUS_CONFIGPARSER_HPP_

#include <iostream>
#include <string>
#include <sstream>		// Conversion between string and primitives
#include <map>			// Pair of key-value
#include <fstream> 		// File Handling
#include <typeinfo>
#include <algorithm>
#include "Miscellaneous/SmartVector.hpp"
#include "Miscellaneous/Convert.hpp"

using namespace std;


class ConfigParser {
public:

	// Constructor
	ConfigParser(const string &);

	// Destructor
	virtual ~ConfigParser();

	template <typename ValueType>
	ValueType GetValueOfKey(const std::string &key, ValueType const &defaultValue = ValueType()) const
	{
		if (!KeyExists(key))
			return defaultValue;

		string value = contents.find(key)->second;

		return Convert::string_to_T<ValueType>(value);
	}

	void Print_Arguments(void)
	{
		for (auto it : contents)
		{
			cout << it.first << " : " << it.second << endl;
		}
	}

	//
	bool KeyExists(const std::string &key) const;

	void UpdateContent(const std::string &key, const std::string &value);

	void AddContent(const std::string &key, const std::string &value); //picks both lvalue and rvalue
	//void AddContent(std::string &key, std::string &value); //picks just lvalue  of key
	//void AddContent(std::string &&key,std::string &value); //picks just rvalue of key

	// Key-Value Pairs
	map<string,string> GetContents(void) { return contents;}

private:

	// Parse File
	bool ExtractKeys();

	// Remove comment from line
	void RemoveComment(string &line) const;

	// Check Whitespace
	bool CheckBlankLine(const string &line) const;

	// Error handling
	void exitWithError(const string &error);

	// Check if that line is valid.
	bool ValidLine(const std::string &line) const;

	// Try to parse current line
	void ParseLine(const string &line, size_t const lineNo);

	//
	void ExtractContents(const string &line);

	//
	void ExtractKey(std::string &key, size_t const &sepPos, const std::string &line) const;

	//
	void ExtractValue(std::string &value, size_t const &sepPos, const std::string &line) const;

	// Key-Value Pairs
	map<string,string> contents;

	// Config File Holder
	string FileName;
};

#endif /* MISCELLANEOUS_CONFIGPARSER_HPP_ */
