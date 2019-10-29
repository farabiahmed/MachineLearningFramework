/*
 * RepresentationEmpty.cpp
 *
 *  Created on: May 28, 2017
 *      Author: farabiahmed
 */

#include <Representations/RepresentationEmpty.hpp>
#include <map>			// Pair of key-value
#include <string>
#include <iostream>
#include <exception>

using namespace std;

RepresentationEmpty::RepresentationEmpty(const Environment& env, const ConfigParser& cfg): Representation(env)
{
	Name = "Representation UDP";

	cout<<"RepresentationEmpty has been initiated. " << endl;
}

RepresentationEmpty::~RepresentationEmpty() {
	// TODO Auto-generated destructor stub
}

pair<int,double> RepresentationEmpty::Get_Greedy_Pair(const SmartVector& state) const
{
	double maxVal = std::numeric_limits<double>::lowest();
	int maxArg=0;

	return make_pair(maxArg,maxVal);
}

SmartVector RepresentationEmpty::Get_Policy(const SmartVector& state) const
{
	// Get Best Policy Index
	pair<int,double> policyInfo = Get_Greedy_Pair(state);

	// Return It
	return environment->Get_Action_List(state)[policyInfo.first];
}

double RepresentationEmpty::Get_Value(const SmartVector& state, const SmartVector& action) const
{
	return 0;
}

void RepresentationEmpty::Set_Value(const SmartVector& state, const SmartVector& action, double value)
{
}

void RepresentationEmpty::Add_Experience(const SmartVector& state, const SmartVector& action, const SmartVector& nextState, const double& reward, const int status)
{
}

void RepresentationEmpty::Print_Value()
{
}
