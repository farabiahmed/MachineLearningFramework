/*
 * QValue.hpp
 *
 *  Created on: Mar 21, 2017
 *      Author: farabiahmed
 */

#ifndef REPRESENTATIONS_TABULARSTATEACTIONPAIR_HPP_
#define REPRESENTATIONS_TABULARSTATEACTIONPAIR_HPP_

#include <cmath>
#include <iomanip>
#include <cassert>
#include <fstream>
#include <iostream>
#include "Environments/Environment.hpp"
#include "Representations/Representation.hpp"
#include "Miscellaneous/ConfigParser.hpp"

/*
 *
 */
class TabularStateActionPair : public Representation {
public:
	TabularStateActionPair(const Environment& env, const ConfigParser& cfg);

	virtual ~TabularStateActionPair();

	pair<int,double> Get_Greedy_Pair(const SmartVector& state) const ;

	double Get_Value(const SmartVector& state, const SmartVector& action) const;

	void Set_Value(const SmartVector& state, const SmartVector& action, double value);

	SmartVector Get_Policy(const SmartVector& state) const;

	void Print_Value();

private:
	// State and Action Pairs
	vector< vector<double> > Qvalue;
};

#endif /* REPRESENTATIONS_TABULARSTATEACTIONPAIR_HPP_ */
