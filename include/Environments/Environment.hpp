/*
 * environment.hpp
 *
 *  Created on: Mar 7, 2017
 *      Author: farabiahmed
 */

#ifndef ENVIRONMENT_ENVIRONMENT_HPP_
#define ENVIRONMENT_ENVIRONMENT_HPP_

#include <iostream>
#include <string>
#include <vector>
#include "Miscellaneous/SmartVector.hpp"
#include "ProbabilityDistributions/ProbabilityDistribution.hpp"
#include "ProbabilityDistributions/DiscreteDistribution.hpp"
//#include "Representations/Representation.hpp"

using namespace std;

/// The learner and decision-maker is called the agent.
/// The thing it interacts with, comprising everything outside the agent, is called the environment.
/// A complete specification of an environment defines a task.

class Representation;


/*! \class Environment
    \brief An abstract Environment class

    This abstract class will provide a base for more
    general classes like Gridworld etc.
*/
class Environment {
public:

	/// Create a constructor
	Environment();

	/// Create a virtual destructor
	virtual ~Environment();

	virtual double Get_Reward(const SmartVector& currentState, const SmartVector& action, const SmartVector& nextState) = 0;

	virtual vector<double> Get_Rewards(){return vector<double>();}

	virtual vector<pair<SmartVector,double>> Get_Transition_Probability(const SmartVector& currentState, const SmartVector& action) = 0;

	virtual vector<SmartVector>& Get_All_Possible_States() const = 0;

	virtual vector<SmartVector>& Get_Action_List(const SmartVector& state) const = 0;

	virtual SmartVector Get_Initial_State() = 0;

	virtual SmartVector Get_Random_State();

	virtual bool Check_Terminal_State(const SmartVector& state) const = 0;

	virtual bool Check_Blocked_State(const SmartVector& state) const = 0;

	virtual SmartVector Get_Next_State(const SmartVector& state, const SmartVector& action) = 0;

	virtual void Display_State(const SmartVector& state) const = 0;

	virtual void Display_Action(const SmartVector& action) const = 0;

	virtual void Display_Policy(const Representation& rep) const = 0;

	virtual void Test(void) = 0;

	string Name;	///< It stores the name of environment to increase readability and awareness.
    				///< Such as GridWorld, Inverted Pendulum etc.

	int number_of_states; 	/// Stores the total number of states.

	int number_of_actions;	/// Stores the total number of actions.

	string representation_model;

	/// Standardization parameters will be used in neural networks.
	vector<double> state_mean; 		/// Stores the standardization bias for each state element.
	vector<double> state_scalar; 	/// Stores the standardization factors for each state element.
	vector<double> action_mean;
	vector<double> action_scalar;
protected:
	enum ActionValue
	{
	    North,
	    East,
		South,
		West,
		Wait,

		TotalNumberOfActions,
	};


};

#endif /* ENVIRONMENT_ENVIRONMENT_HPP_ */
