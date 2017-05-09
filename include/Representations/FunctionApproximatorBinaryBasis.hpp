/*
 * FunctionApproximatorBinaryBasis.hpp
 *
 *  Created on: May 8, 2017
 *      Author: farabiahmed
 */

#ifndef REPRESENTATIONS_FUNCTIONAPPROXIMATORBINARYBASIS_HPP_
#define REPRESENTATIONS_FUNCTIONAPPROXIMATORBINARYBASIS_HPP_

#include <cmath>
#include <iomanip>
#include <cassert>
#include <limits>
#include <iostream>
#include "Environments/Environment.hpp"
#include "Representations/Representation.hpp"
#include "Miscellaneous/ConfigParser.hpp"

/*
 *
 */
class FunctionApproximatorBinaryBasis : public Representation{
public:
	FunctionApproximatorBinaryBasis(const Environment& env, const ConfigParser& cfg);

	virtual ~FunctionApproximatorBinaryBasis();

	pair<int,double> Get_Greedy_Pair(const SmartVector& state) const;

	SmartVector Get_Policy(const SmartVector& state) const;

	double Get_Value(const SmartVector& state, const SmartVector& action) const;

	void Set_Value(const SmartVector& state, const SmartVector& action, double value);

	void Print_Value();

	virtual SmartVector Get_Phi(const SmartVector& state,const SmartVector& action) const;

protected:

	vector<SmartVector> features;  /// Locations of radial basis functions.

	int numberOfActions;	/// Total number of actions that forms the phi and theta.

	SmartVector theta;		/// Weights for Radial basis functions
							/// QValue = (phi')*(theta).

	double alpha; 			/// Gradient Descent Learning Rate
							/// Parameter denotes the learning rate of gradient descent optimization algorithm
};

#endif /* REPRESENTATIONS_FUNCTIONAPPROXIMATORBINARYBASIS_HPP_ */
