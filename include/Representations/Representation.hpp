/*
 * Representation.hpp
 *
 *  Created on: Mar 21, 2017
 *      Author: farabiahmed
 */

#ifndef REPRESENTATIONS_REPRESENTATION_HPP_
#define REPRESENTATIONS_REPRESENTATION_HPP_

#include <list>
#include <vector>
//#include "Environments/Environment.hpp"
#include "Miscellaneous/SmartVector.hpp"

// Forward Declaration to avoid inclusion chain errors.
class Environment;

/*
 *
 */
class Representation {
public:

	Representation(const Environment& env);

	virtual ~Representation();

	virtual pair<int,double> Get_Greedy_Pair(const SmartVector& state) const = 0;

	virtual SmartVector Get_Policy(const SmartVector& state) const = 0;

	virtual double Get_Value(const SmartVector& state, const SmartVector& action) const = 0;

	virtual void Set_Value(const SmartVector& state, const SmartVector& action, double value) = 0;

	virtual void Print_Value() = 0;

protected:
	const Environment* environment;
};

#endif /* REPRESENTATIONS_REPRESENTATION_HPP_ */
