/*
 * RepresentationUDP.hpp
 *
 *  Created on: May 28, 2017
 *      Author: farabiahmed
 */

#ifndef REPRESENTATIONS_REPRESENTATIONUDP_HPP_
#define REPRESENTATIONS_REPRESENTATIONUDP_HPP_

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
class RepresentationUDP : public  Representation {
public:
	RepresentationUDP(const Environment& env, const ConfigParser& cfg);

	virtual ~RepresentationUDP();

	pair<int,double> Get_Greedy_Pair(const SmartVector& state) const;

	SmartVector Get_Policy(const SmartVector& state) const;

	double Get_Value(const SmartVector& state, const SmartVector& action) const;

	void Set_Value(const SmartVector& state, const SmartVector& action, double value);

	void Add_Experience(const SmartVector& state, const SmartVector& action, const SmartVector& nextState, const double& reward, const int status);

	void Print_Value();

	SmartVector Initial_State(const SmartVector& state, double fitnessValue, bool& isInitialStateDone) const;

	string Name;	///< It stores the name of representation to increase readability and awareness.
    				///< Such as StateActionValue, GaussianRBF etc.

protected:
	string Vector_ToString(const SmartVector& vec) const;

	UDP udpsocket;

private:

	string host_ip;

	int port_tx;

	int port_rx;

	double gamma;

};

#endif /* REPRESENTATIONS_REPRESENTATIONUDP_HPP_ */
