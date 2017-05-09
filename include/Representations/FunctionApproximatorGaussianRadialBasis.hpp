/*
 * FunctionApproximatorGaussianRadialBasis.hpp
 *
 *  Created on: May 9, 2017
 *      Author: farabiahmed
 */

#ifndef REPRESENTATIONS_FUNCTIONAPPROXIMATORGAUSSIANRADIALBASIS_HPP_
#define REPRESENTATIONS_FUNCTIONAPPROXIMATORGAUSSIANRADIALBASIS_HPP_


#include "Representations/FunctionApproximatorBinaryBasis.hpp"

/*
 *
 */
class FunctionApproximatorGaussianRadialBasis : public FunctionApproximatorBinaryBasis{
public:
	FunctionApproximatorGaussianRadialBasis(const Environment& env, const ConfigParser& cfg);
	virtual ~FunctionApproximatorGaussianRadialBasis();
	SmartVector Get_Phi(const SmartVector& state,const SmartVector& action) const;
};

#endif /* REPRESENTATIONS_FUNCTIONAPPROXIMATORGAUSSIANRADIALBASIS_HPP_ */
