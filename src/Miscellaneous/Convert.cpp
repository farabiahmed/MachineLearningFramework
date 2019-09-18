/*
 * Convert.cpp
 *
 *  Created on: Apr 30, 2017
 *      Author: farabiahmed
 */

#include <Miscellaneous/Convert.hpp>


template <>
SmartVector Convert::string_to_T <SmartVector> (string const &val)
{
	if(val=="N/A")
		return SmartVector();

	try{
		vector<string> tokens = Parse_String(val,',');
		SmartVector ret(tokens.size());

		for (unsigned i = 0; i < tokens.size(); ++i) {
			ret.elements[i] = (double)stod(tokens[i]);
		}


		return ret;
	}
	catch (int e)
	{
		cout << "An exception occurred. Exception Code: " << e << '\n';
		cout << "Inputs: " << val << '\n';

		exit(0);
	}

}

template <>
vector<SmartVector> Convert::string_to_T < vector<SmartVector> > (string const &val)
{
	if(val=="N/A")
		return vector<SmartVector>();

	vector<SmartVector> ret;
	vector<string> tokens = Parse_String(val,';');

	for (unsigned i = 0; i < tokens.size(); ++i) {
		ret.push_back(string_to_T <SmartVector> (tokens[i]));
	}

	return ret;
}

template <>
vector<string> Convert::string_to_T < vector<string> > (string const &val)
{
	if(val=="N/A")
		return vector<string>();

	vector<string> tokens = Parse_String(val,',');

	return tokens;
}

template <>
vector<double> Convert::string_to_T <vector<double>> (string const &val)
{
	if(val=="N/A")
		return vector<double>();

	vector<double> ret;
	vector<string> tokens = Parse_String(val,',');

	for (unsigned i = 0; i < tokens.size(); ++i) {
		ret.push_back((double)stod(tokens[i]));
	}

	return ret;
}

template <>
vector<int> Convert::string_to_T <vector<int>> (string const &val)
{
	if(val=="N/A")
		return vector<int>();

	vector<int> ret;
	vector<string> tokens = Parse_String(val,',');

	for (unsigned i = 0; i < tokens.size(); ++i) {
		ret.push_back((int)stoi(tokens[i]));
	}

	return ret;
}

template <>
vector<unsigned> Convert::string_to_T <vector<unsigned>> (string const &val)
{
	if(val=="N/A")
		return vector<unsigned>();

	vector<unsigned> ret;
	vector<string> tokens = Parse_String(val,',');

	for (unsigned i = 0; i < tokens.size(); ++i) {
		ret.push_back((unsigned)stoi(tokens[i]));
	}

	return ret;
}


template <>
vector<vector<int>> Convert::string_to_T < vector<vector<int>> > (string const &val)
{
	if(val=="N/A")
		return vector<vector<int>>();

	vector<vector<int>> ret;
	vector<string> tokens = Parse_String(val,';');

	for (unsigned i = 0; i < tokens.size(); ++i) {
		ret.push_back(string_to_T <vector<int>> (tokens[i]));
	}

	return ret;
}


template <>
vector<vector<unsigned>> Convert::string_to_T < vector<vector<unsigned>> > (string const &val)
{
	if(val=="N/A")
		return vector<vector<unsigned>>();

	vector<vector<unsigned>> ret;
	vector<string> tokens = Parse_String(val,';');

	for (unsigned i = 0; i < tokens.size(); ++i) {
		ret.push_back(string_to_T <vector<unsigned>> (tokens[i]));
	}

	return ret;
}


vector<string> Convert::Parse_String(const string& val, const char& delimiter)
{
	vector<string> str_arr;

	string str = val;

	size_t pos = 0;

	while( (pos=str.find(delimiter)) !=  string::npos)
	{
		str_arr.push_back(str.substr(0,pos));
		str = str.erase(0,pos+1);
	}
	str_arr.push_back(str);

	return str_arr;
}


