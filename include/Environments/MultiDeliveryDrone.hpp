/*
 * Rendezvous.hpp
 *
 *  Created on: Sep 16, 2019
 *      Author: farabiahmed
 */

#ifndef ENVIRONMENTS_MULTI_DELIVERY_DRONE_HPP_
#define ENVIRONMENTS_MULTI_DELIVERY_DRONE_HPP_

#include "Environments/Environment.hpp"
#include "Environments/DeliveryDrone.hpp"
#include "Miscellaneous/ConfigParser.hpp"
#include "Representations/Representation.hpp"

//template <class T=Gridworld > //Conditional Inheritance requires all the implementation in same file :(.
class 	MultiDeliveryDrone : public DeliveryDrone
{
public:

	MultiDeliveryDrone(const ConfigParser& cfg);

	~MultiDeliveryDrone();

	SmartVector Get_Initial_State();

	SmartVector Get_Random_State();

	bool ComparePosition(const SmartVector& state1, const SmartVector& state2) const;

	double Get_Reward(const SmartVector& currentState, const SmartVector& action, const SmartVector& nextState);
	virtual vector<double> Get_Rewards();

	vector<pair<SmartVector,double>> Get_Transition_Probability(const SmartVector& currentState, const SmartVector& action);

//	vector<SmartVector> Get_All_Possible_States() const;

	vector<SmartVector>& Get_Action_List(const SmartVector&) const;

	int Get_State_Index(const SmartVector& state) const;

	SmartVector Get_Next_State(const SmartVector& state, const SmartVector& action);

	bool Check_Terminal_State(const SmartVector& state) const;

	bool Check_Blocked_State(const SmartVector& state) const;

	void Display_Policy(const Representation& rep)  const;

	void Display_State(const SmartVector& state) const;

private:

	unsigned number_of_agents;

	const int DIMENSION_FOR_AGENT;

	vector<double> rewards;

};

#endif /* ENVIRONMENTS_MULTI_DELIVERY_DRONE_HPP_ */
