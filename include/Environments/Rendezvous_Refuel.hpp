/*
 * Rendezvous.hpp
 *
 *  Created on: Sep 16, 2019
 *      Author: farabiahmed
 */

#ifndef ENVIRONMENTS_RENDEZVOUS_REFUEL_HPP_
#define ENVIRONMENTS_RENDEZVOUS_REFUEL_HPP_

#include "Environments/Environment.hpp"
#include "Environments/Gridworld_Refuel.hpp"
#include "Miscellaneous/ConfigParser.hpp"
#include "Representations/Representation.hpp"

//template <class T=Gridworld > //Conditional Inheritance requires all the implementation in same file :(.
class 	Rendezvous_Refuel : public Gridworld_Refuel
{
public:

	Rendezvous_Refuel(const ConfigParser& cfg);

	~	Rendezvous_Refuel();

	SmartVector Get_Initial_State();

	//SmartVector Get_Random_State();

	double Get_Reward(const SmartVector& currentState, const SmartVector& action, const SmartVector& nextState);

	vector<pair<SmartVector,double>> Get_Transition_Probability(const SmartVector& currentState, const SmartVector& action);

	vector<SmartVector> Get_All_Possible_States() const;

	vector<SmartVector> Get_Action_List(const SmartVector&) const;

	int Get_State_Index(const SmartVector& state) const;

	SmartVector Get_Next_State(const SmartVector& state, const SmartVector& action);

	bool Check_Terminal_State(const SmartVector& state) const;

	bool Check_Blocked_State(const SmartVector& state) const;

	void Display_Policy(const Representation& rep)  const;

	void Display_State(const SmartVector& state) const;

private:

	unsigned number_of_agents;

	vector<SmartVector> initial_state;
};

#endif /* ENVIRONMENTS_RENDEZVOUS_REFUEL_HPP_ */
