/*
 * GaussianRadialBasisFunctionApproximator.hpp
 *
 *  Created on: May 8, 2017
 *      Author: farabiahmed
 */

#ifndef REPRESENTATIONS_GAUSSIANRADIALBASISFUNCTIONAPPROXIMATOR_HPP_
#define REPRESENTATIONS_GAUSSIANRADIALBASISFUNCTIONAPPROXIMATOR_HPP_

#include <cmath>
#include <iomanip>
#include <cassert>
#include "Environments/Environment.hpp"
#include "Representations/Representation.hpp"
#include "Miscellaneous/ConfigParser.hpp"
/*
 *
 */
class GaussianRadialBasisFunctionApproximator : public Representation{
public:
	GaussianRadialBasisFunctionApproximator(const Environment& env, const ConfigParser& cfg);

	virtual ~GaussianRadialBasisFunctionApproximator();

	pair<int,double> Get_Greedy_Pair(SmartVector state) const;

	SmartVector Get_Policy(SmartVector state);

	double Get_Value(const SmartVector& state, const SmartVector& action);

	void Set_Value(const SmartVector& state, const SmartVector& action, double value);

	void Print_Value();
};

#endif /* REPRESENTATIONS_GAUSSIANRADIALBASISFUNCTIONAPPROXIMATOR_HPP_ */
