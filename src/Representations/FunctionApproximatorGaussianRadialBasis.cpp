/*
 * FunctionApproximatorGaussianRadialBasis.cpp
 *
 *  Created on: May 9, 2017
 *      Author: farabiahmed
 */

#include <Representations/FunctionApproximatorGaussianRadialBasis.hpp>

FunctionApproximatorGaussianRadialBasis::FunctionApproximatorGaussianRadialBasis(const Environment& env, const ConfigParser& cfg)
:FunctionApproximatorBinaryBasis(env,cfg)
{
	// TODO Auto-generated constructor stub

}

FunctionApproximatorGaussianRadialBasis::~FunctionApproximatorGaussianRadialBasis() {
	// TODO Auto-generated destructor stub
}

// Get Phi Using Radial Basis Functions (RBFs)
SmartVector FunctionApproximatorGaussianRadialBasis::Get_Phi(const SmartVector& state,const SmartVector& action) const
{
	// Create a local phi variable to return when we are OK.
	SmartVector phi(features.size()*numberOfActions);

	// Set whole vector to zero
	phi.Initialize();

	// Get the right feature_index to start to load
	int feature_start_index = (features.size() * action.index);

	// Container that holds the values of RBFs.
	double feature_values[features.size()];

	for (unsigned i = 0; i < features.size(); ++i) {

		SmartVector diff = state - features[i];

		double temp = 0;

		// Calculate (x-xc1)^2 + (x2-xc2)^2 ...
		for (int j = 0; j < state.size(); ++j) {
			temp += pow(diff.elements[j],2);
		}

		// Calculate e^(-Beta*x^2)
		feature_values[i] = exp( -1 * temp);

		//cout<<feature_values[i]<<endl;

		// Set the related feature
		phi.elements[feature_start_index + i] = feature_values [i];
	}

	return phi;
}


