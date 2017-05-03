/*
 * QValue.hpp
 *
 *  Created on: Mar 21, 2017
 *      Author: farabiahmed
 */

#ifndef REPRESENTATIONS_STATEACTIONVALUE_HPP_
#define REPRESENTATIONS_STATEACTIONVALUE_HPP_

#include <cmath>
#include <iomanip>
#include <cassert>
#include "Environments/Environment.hpp"
#include "Representations/Representation.hpp"
#include "Miscellaneous/ConfigParser.hpp"

/*
 *
 */
class StateActionValue : public Representation {
public:
	StateActionValue(const Environment& env, const ConfigParser& cfg);

	virtual ~StateActionValue();

	pair<int,double> Get_Greedy_Pair(SmartVector state) const ;

	list<double> Get_Value(SmartVector state);

	double Get_Value(const SmartVector& state, const SmartVector& action);

	void Set_Value(const SmartVector& state, const SmartVector& action, double value);

	SmartVector Get_Policy(SmartVector state);

	void Print_Value();

private:
};

#endif /* REPRESENTATIONS_STATEACTIONVALUE_HPP_ */
