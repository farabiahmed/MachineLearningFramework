/*
 * Blocksworld.hpp
 *
 *  Created on: Apr 26, 2017
 *      Author: farabiahmed
 */

#ifndef ENVIRONMENTS_BLOCKSWORLD_HPP_
#define ENVIRONMENTS_BLOCKSWORLD_HPP_


#include <iomanip>
#include <cmath>
#include "Environments/Environment.hpp"
#include "Miscellaneous/ConfigParser.hpp"

/// State (n x 1):
/// 		- x = [block1, block2, ..., blockn] where n is number of total blocks.
/// 		- i is an element of {1,2,...n}
/// 		- blocki indicates the position of ith block. It can be over ground or any other block.
/// 		- blocki is an element of {0,1,...,i,...,n} except i.
/// 		- blocki can be on the ground(0) or any other block except itself.
///
///
/// Action (2 x 1):
/// 		- (0,N/A) means do nothing
/// 		- (i,s) means stack ith block to top of given slot s where i>0, s={0,1,...,n-1}
/// 		- (j,N/A) means unstack |j|th block from wherever it is, and then place it to a first empty slot, where j<0
///			- N/A: Not applicable, means its value is not important.
class Blocksworld : public Environment
{
public:

	Blocksworld(const ConfigParser& cfg);

	virtual ~Blocksworld();

	double Get_Reward(const SmartVector& currentState, const SmartVector& action, const SmartVector& nextState);

	vector<pair<SmartVector,double>> Get_Transition_Probability(const SmartVector& currentState, const SmartVector& action);

	vector<SmartVector> Get_All_Possible_States() const;

	vector<SmartVector> Get_Action_List(const SmartVector& state) const;

	SmartVector Get_Initial_State();

	bool Check_Terminal_State(const SmartVector& state) const;

	bool Check_Blocked_State(const SmartVector& state) const;

	SmartVector Get_Goal_State(void) const;

	SmartVector Get_Next_State(const SmartVector& state, const SmartVector& action);

	void Display_State(const SmartVector& state) const;

	void Display_Action(const SmartVector& action) const;

	void Display_Policy(const Representation& rep) const;

	vector<vector<int>> Get_Slots( const SmartVector& state)  const;

	int Get_State_Index(const SmartVector& state) const;

	void Test(void);

private:
	unsigned number_of_blocks;
	double reward_goal;
	double cost_action;
	double probability_of_random_action;
};

#endif /* ENVIRONMENTS_BLOCKSWORLD_HPP_ */
