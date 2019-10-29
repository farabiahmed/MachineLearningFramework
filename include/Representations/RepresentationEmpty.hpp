/*
 * RepresentationUDP.hpp
 *
 *  Created on: May 28, 2017
 *      Author: farabiahmed
 */

#ifndef REPRESENTATIONS_REPRESENTATIONEMPTY_HPP_
#define REPRESENTATIONS_REPRESENTATIONEMPTY_HPP_

#include <cmath>
#include <iomanip>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Environments/Environment.hpp"
#include "Representations/Representation.hpp"
#include "Miscellaneous/ConfigParser.hpp"
#include "Communication/UDP.hpp"
#include "Miscellaneous/Convert.hpp"

/*
 *
 */
class RepresentationEmpty : public  Representation {
public:
	RepresentationEmpty(const Environment& env, const ConfigParser& cfg);

	virtual ~RepresentationEmpty();

	pair<int,double> Get_Greedy_Pair(const SmartVector& state) const;

	SmartVector Get_Policy(const SmartVector& state) const;

	double Get_Value(const SmartVector& state, const SmartVector& action) const;

	void Set_Value(const SmartVector& state, const SmartVector& action, double value);

	void Add_Experience(const SmartVector& state, const SmartVector& action, const SmartVector& nextState, const double& reward, const int status);

	void Print_Value();

	string Name;	///< It stores the name of representation to increase readability and awareness.
    				///< Such as StateActionValue, GaussianRBF etc.

};

#endif /* REPRESENTATIONS_REPRESENTATIONEMPTY_HPP_ */
