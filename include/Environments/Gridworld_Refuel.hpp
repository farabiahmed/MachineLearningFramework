/*
 * gridworld.hpp
 *
 *  Created on: Sep, 16, 2019
 *      Author: farabiahmed
 */

#ifndef GRIDWORLD_REFUEL_HPP_
#define GRIDWORLD_REFUEL_HPP_

#include "Environments/Environment.hpp"
#include "Miscellaneous/ConfigParser.hpp"
#include "Representations/Representation.hpp"

//class environment; //forward declaration

/*
 *
 */
class Gridworld_Refuel : public Environment
{
public:

	Gridworld_Refuel(const ConfigParser& cfg);

	~Gridworld_Refuel();

	double Get_Reward(const SmartVector&, const SmartVector&, const SmartVector&);

	vector<pair<SmartVector,double>> Get_Transition_Probability(const SmartVector& currentState, const SmartVector& action);

	SmartVector BurnFuel(const SmartVector &state);

	bool ComparePosition(const SmartVector& state1, const SmartVector& state2) const;

	vector<SmartVector> Get_All_Possible_States() const;

	vector<SmartVector> Get_Action_List(const SmartVector&) const;

	bool Get_Feasibility_Of_Action(const SmartVector& state, const SmartVector& action) const;

	SmartVector Get_Initial_State();

	bool Check_Terminal_State(const SmartVector& state) const;

	bool Check_Blocked_State(const SmartVector& state) const;

	bool Check_Refuel_State(const SmartVector& state) const;

	SmartVector Get_Next_State(const SmartVector& state, const SmartVector& action);

	int Get_State_Index(const SmartVector& state) const;

	void Display_State(const SmartVector& state) const;

	void Display_Action(const SmartVector& action) const;

	void Display_Policy(const Representation& rep) const;

	void Test(void);

protected:

	vector<SmartVector> terminal_states;

	vector<SmartVector> blocked_states;

	vector<SmartVector> refuel_states;

	vector<double> rewards_of_terminal_states;

	double cost_action;

	double probability_of_random_action;

	int number_of_columns;

	int number_of_rows;

	int fuel_initial;

	int fuel_max;
};

#endif /* GRIDWORLD_REFUEL_HPP_ */
