/*
 * gridworld.hpp
 *
 *  Created on: Mar 7, 2017
 *      Author: farabiahmed
 */

#ifndef GRIDWORLD_HPP_
#define GRIDWORLD_HPP_

#include "Environments/Environment.hpp"
#include "Miscellaneous/ConfigParser.hpp"
#include "Representations/Representation.hpp"

//class environment; //forward declaration

/*
 *
 */
class Gridworld : public Environment
{
public:

	Gridworld(const ConfigParser& cfg);

	~Gridworld();

	double Get_Reward(const SmartVector&, const SmartVector&, const SmartVector&);

	vector<pair<SmartVector,double>> Get_Transition_Probability(const SmartVector& currentState, const SmartVector& action);

	vector<SmartVector> Get_All_Possible_States() const;

	vector<SmartVector> Get_Action_List(const SmartVector&) const;

	bool Get_Feasibility_Of_Action(const SmartVector& state, const SmartVector& action) const;

	SmartVector Get_Initial_State();

	bool Check_Terminal_State(const SmartVector& state) const;

	bool Check_Blocked_State(const SmartVector& state) const;

	SmartVector Get_Next_State(const SmartVector& state, const SmartVector& action);

	int Get_State_Index(const SmartVector& state) const;

	void Display_State(const SmartVector& state) const;

	void Display_Action(const SmartVector& action) const;

	void Display_Policy(const Representation& rep) const;

	void Test(void);

protected:

	vector<SmartVector> terminal_states;

	vector<SmartVector> blocked_states;

	vector<double> rewards_of_terminal_states;

	double cost_action;

	double probability_of_random_action;

	int number_of_columns;

	int number_of_rows;
};

#endif /* GRIDWORLD_HPP_ */
