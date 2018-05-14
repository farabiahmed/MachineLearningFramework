/*
 * RepresentationActorCritic.h
 *
 *  Created on: May 13, 2018
 *      Author: farabiahmed
 */

#ifndef REPRESENTATIONS_REPRESENTATIONACTORCRITICTABULAR_HPP_
#define REPRESENTATIONS_REPRESENTATIONACTORCRITICTABULAR_HPP_


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
class RepresentationActorCriticTabular : public Representation {
public:
	RepresentationActorCriticTabular(const Environment& env, const ConfigParser& cfg);

	virtual ~RepresentationActorCriticTabular();

	pair<int,double> Get_Greedy_Pair(const SmartVector& state) const ;

	double Get_Value(const SmartVector& state, const SmartVector& action) const;

	void Set_Value(const SmartVector& state, const SmartVector& action, double value);

	SmartVector Get_Policy(const SmartVector& state) const;

	void Print_Value();

private:

	int SoftMax(vector<double> &values) const;

	// State and Action Pairs
	vector< vector<double> > ActorValue;

	// Utility Matrix
	vector< double > CriticValue;
};


#endif /* REPRESENTATIONS_REPRESENTATIONACTORCRITICTABULAR_HPP_ */
