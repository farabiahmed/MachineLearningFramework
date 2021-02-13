/*
 * MultiDeliveryDrone.cpp
 *
 *  Created on: Jul 26, 2017
 *      Author: farabiahmed
 */

#include <Environments/MultiDeliveryDrone.hpp>

MultiDeliveryDrone::MultiDeliveryDrone(const ConfigParser& cfg)
:DeliveryDrone(cfg),
 DIMENSION_FOR_AGENT(DeliveryDrone::TotalNumberOfStates)// x,y,fuel,packet,delivery
{
	number_of_agents = cfg.GetValueOfKey<int>("NUMBER_OF_AGENTS");
}

MultiDeliveryDrone::~MultiDeliveryDrone()
{

}

// FIXED: Fix for multi state agents: x,y,fuel,
// TODO: get the states from base class and get their combination.
//vector<SmartVector> MultiDeliveryDrone::Get_All_Possible_States() const
//{
//	cout<<endl<<endl<<"NOT IMPLEMENTED YET"<<  __func__ <<endl<<endl;
//	abort();
//}

bool MultiDeliveryDrone::Check_Terminal_State(const SmartVector& state) const
{
	if((unsigned)state.dimension == number_of_agents * DeliveryDrone::TotalNumberOfStates)
	{
		vector<SmartVector> splittedStates = SmartVector::Split(state,number_of_agents);

		for (unsigned i = 0; i < number_of_agents; ++i)
		{
			if(!DeliveryDrone::Check_Terminal_State(splittedStates[i]))
				return false;
		}
		return true;
	}
	else
		return DeliveryDrone::Check_Terminal_State(state);
}

//TODO: needs to be fixed for multiblock
bool MultiDeliveryDrone::Check_Blocked_State(const SmartVector& state) const
{
	if((unsigned)state.dimension == number_of_agents * DeliveryDrone::TotalNumberOfStates)
	{
		vector<SmartVector> splittedStates = SmartVector::Split(state,number_of_agents);

		for (size_t i = 0; i < splittedStates.size(); ++i)
		{
			if(DeliveryDrone::Check_Blocked_State(splittedStates[i]))
			{
				return true;
			}
		}
		return false;
	}
	else
		return DeliveryDrone::Check_Blocked_State(state);
}

//Fixed: needs to be updated for parametric multiagent case.
vector<SmartVector>& MultiDeliveryDrone::Get_Action_List(const SmartVector& state) const
{
	static vector<SmartVector> actions;

	unsigned index=0;
	if(actions.size()==0)
	{
		size_t size_of_actions = pow(number_of_actions, number_of_agents);

		for (size_t i = 0; i < size_of_actions; ++i)
		{
			SmartVector action(number_of_agents);

			for (int j = 0; j < action.size(); j++)
			{
				/*
				 *	3 Agent Case:
				 	action[0] = ActionA
				 	action[1] = ActionB
				 	action[2] = ActionC

				 *	Parametric 3 Agent Case:
				 	action[0] = (i / number_of_actions * number_of_actions) % number_of_actions
				 	action[1] = (i / number_of_actions) % number_of_actions
				 	action[2] = (i / 1) % number_of_actions
				 */

				unsigned currentAgentId = j;

				unsigned numberOfFieldsToPass = pow(number_of_actions, (number_of_agents - currentAgentId - 1));

				action.elements[j] 	= (i / numberOfFieldsToPass ) 	% number_of_actions;

			}
			action.index = index++;
			actions.push_back(action);
		}
	}

	return actions;

}

bool MultiDeliveryDrone::ComparePosition(const SmartVector& state1, const SmartVector& state2) const
{
	for (size_t i = 0; i < number_of_agents; ++i) {
		if(!(
			(state1.elements[i*DIMENSION_FOR_AGENT]==state2.elements[i*DIMENSION_FOR_AGENT]) &&
			(state1.elements[i*DIMENSION_FOR_AGENT+1]==state2.elements[i*DIMENSION_FOR_AGENT+1])
			)
		  )
		{
			return false;
		}
	}
	return true;
}

double MultiDeliveryDrone::Get_Reward(const SmartVector& currentState, const SmartVector& action, const SmartVector& nextState)
{
	double reward = 0;

	/*
	if(Check_Terminal_State(nextState))
	{
		reward += 1;

		return reward + cost_action;
	}
	*/

	vector<SmartVector> currentStates = SmartVector::Split(currentState,number_of_agents);
	vector<SmartVector> actions = SmartVector::Split(action,number_of_agents);
	vector<SmartVector> nextStates = SmartVector::Split(nextState,number_of_agents);

	for (unsigned i = 0; i < number_of_agents; ++i) {
		reward += DeliveryDrone::Get_Reward(currentStates[i], actions[i], nextStates[i]);
	}

	return reward/(double)number_of_agents;
}

//FIXED: Fix to multiagent case for n>2
SmartVector MultiDeliveryDrone::Get_Initial_State()
{
	cout<<endl<<endl<<"NOT IMPLEMENTED YET"<<  __func__ <<endl<<endl;
	abort();
}

// Random state should be active. Otherwise, some state from all possible states can be choosen even it has zero fuel. 
SmartVector MultiDeliveryDrone::Get_Random_State()
{
	SmartVector newStateComposed;
	vector<SmartVector> nextStates(number_of_agents);
	for (unsigned i = 0; i < number_of_agents; ++i) {

		nextStates[i] = DeliveryDrone::Get_Random_State();
		newStateComposed = SmartVector::Combine(newStateComposed,nextStates[i]);
	}

	return newStateComposed;
}


// TODO:FAILURE FOR 3AGENT!
vector<pair<SmartVector,double>> MultiDeliveryDrone::Get_Transition_Probability(const SmartVector& currentState, const SmartVector& action)
{
	cout<<endl<<endl<<"NOT IMPLEMENTED YET"<<  __func__ <<endl<<endl;
	abort();
}

//TODO Use Hashmaps instead of bruteforce search
int MultiDeliveryDrone::Get_State_Index(const SmartVector& state) const
{
	if(representation_model != "TabularStateActionPair")
		return -1;

	static vector<SmartVector> states = MultiDeliveryDrone::Get_All_Possible_States();

	for (unsigned i = 0; i < states.size(); ++i) {
		if(state == states[i])
			return i;
	}

	cout<<endl<<endl<<"INVALID STATE!"<<endl<<endl;

	state.Print();

	cout<<"Controlled Abort..."<<endl;
	abort();
	return -1;
}


SmartVector MultiDeliveryDrone::Get_Next_State(const SmartVector& state, const SmartVector& action)
{
	// Decompose states and actions
	vector<SmartVector> states = SmartVector::Split(state,number_of_agents);
	vector<SmartVector> actions = SmartVector::Split(action,number_of_agents);

	// Create next states and fill it from super class.
	vector<SmartVector> nextStates;

	for (unsigned i = 0; i < number_of_agents; ++i) {
		states[i].index = DeliveryDrone::Get_State_Index(states[i]);

		SmartVector nextstate = DeliveryDrone::Get_Next_State(states[i],actions[i]);

		nextStates.push_back(nextstate);
	}

	SmartVector newStateComposed;
	for (unsigned i = 0; i < number_of_agents; ++i) {
		newStateComposed = SmartVector::Combine(newStateComposed,nextStates[i]);
	}

	newStateComposed.index = Get_State_Index(newStateComposed);
	return newStateComposed;
}

void MultiDeliveryDrone::Display_Policy(const Representation& rep)  const
{
	cout<<endl<<endl<<"NOT IMPLEMENTED YET"<<  __func__ <<endl<<endl;
}

void MultiDeliveryDrone::Display_State(const SmartVector& state) const
{
	vector<SmartVector> agent_states = SmartVector::Split(state,number_of_agents);

	cout<<"Displaying Multiagent State Status: [";
	for (unsigned i = 0; i < number_of_agents * StateIdx::TotalNumberOfStates; ++i) {
		cout<< " " << state.elements[i];
	}
	cout<<" ] #"<<state.index<<endl<<endl;

	for (unsigned a = 0; a < number_of_agents; ++a) {

		cout<<"Displaying State for Agent #"<<a<<endl;

		DeliveryDrone::Display_State(agent_states[a]);

		cout<<endl<<endl;
	}
}
