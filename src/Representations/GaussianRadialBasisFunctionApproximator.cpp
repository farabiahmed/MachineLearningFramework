/*
 * GaussianRadialBasisFunctionApproximator.cpp
 *
 *  Created on: May 8, 2017
 *      Author: farabiahmed
 */

#include <Representations/GaussianRadialBasisFunctionApproximator.hpp>

GaussianRadialBasisFunctionApproximator::GaussianRadialBasisFunctionApproximator(const Environment& env, const ConfigParser& cfg)
:Representation(env)
{
	// TODO Auto-generated constructor stub

}

GaussianRadialBasisFunctionApproximator::~GaussianRadialBasisFunctionApproximator() {
	// TODO Auto-generated destructor stub
}

pair<int,double> GaussianRadialBasisFunctionApproximator::Get_Greedy_Pair(SmartVector state) const
{
	int maxArg;
	double maxVal;

	return make_pair(maxArg,maxVal);
}

SmartVector GaussianRadialBasisFunctionApproximator::Get_Policy(SmartVector state)
{
	SmartVector policy;

	return policy;
}

double GaussianRadialBasisFunctionApproximator::Get_Value(const SmartVector& state, const SmartVector& action)
{

	return 0;
}

void GaussianRadialBasisFunctionApproximator::Set_Value(const SmartVector& state, const SmartVector& action, double value)
{

}

void GaussianRadialBasisFunctionApproximator::Print_Value()
{

}
