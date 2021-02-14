/*
 * Representation.hpp
 *
 *  Created on: Mar 21, 2017
 *      Author: farabiahmed
 */

#ifndef REPRESENTATIONS_REPRESENTATION_HPP_
#define REPRESENTATIONS_REPRESENTATION_HPP_

#include <fstream> // file write for datalog
#include <cstdlib> // system command
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

	virtual pair<vector<int>,vector<double>> Get_Greedy_Pairs(const SmartVector& state) const {return make_pair(vector<int>(),vector<double>());}

	virtual SmartVector Get_Policy(const SmartVector& state) const = 0;

	virtual double Get_Value(const SmartVector& state, const SmartVector& action) const = 0;

	virtual void Set_Value(const SmartVector& state, const SmartVector& action, double value) = 0;

	virtual void Set_Values(const SmartVector& state, const SmartVector& action, vector<double> values){}

	virtual void Print_Value() = 0;

	virtual void Add_Experience(const SmartVector& state, const SmartVector& action, const SmartVector& nextState, const double& reward, const int status){};

	virtual void Get_Report(string filePath, string fileName);

	virtual SmartVector Initial_State(const SmartVector& state, double fitnessValue, bool& initialStateDone) const {return state;}

	string Name;	///< It stores the name of representation to increase readability and awareness.
    				///< Such as StateActionValue, GaussianRBF etc.

protected:
	const Environment* environment;
};

#endif /* REPRESENTATIONS_REPRESENTATION_HPP_ */
