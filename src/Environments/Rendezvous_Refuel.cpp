/*
 * Rendezvous_Refuel.cpp
 *
 *  Created on: Jul 26, 2017
 *      Author: farabiahmed
 */

#include <Environments/Rendezvous_Refuel.hpp>

Rendezvous_Refuel::Rendezvous_Refuel(const ConfigParser& cfg)
:Gridworld_Refuel(cfg)
{
	number_of_agents = cfg.GetValueOfKey<int>("NUMBER_OF_AGENTS");

	initial_state = cfg.GetValueOfKey<vector<SmartVector>>("INITIAL_STATE");

	for (size_t i = 0; i < number_of_agents; ++i) {
		initial_state[i].elements[2] = fuel_max;
	}
}

Rendezvous_Refuel::~Rendezvous_Refuel()
{

}

// FIXED: Fix for multi state agents: x,y,fuel,
// TODO: get the states from base class and get their combination.
vector<SmartVector> Rendezvous_Refuel::Get_All_Possible_States() const
{
	const static int DIMENSION_FOR_AGENT = 3; // x,y,fuel

	static vector<SmartVector> states;		//Cache states since it is expensive to calculate

	int index=0;

	int dimension_fuel = fuel_max + 1;

	if(states.size()==0)
	{
		size_t size_of_states = pow(number_of_rows * number_of_columns * dimension_fuel, number_of_agents);

		for (size_t i = 0; i < size_of_states; ++i)
		{
			SmartVector state(DIMENSION_FOR_AGENT * number_of_agents);

			for (int j = 0; j < state.size(); j+=DIMENSION_FOR_AGENT) {

				/*
				 	* 3 Agent case
					state.elements[0] = RowA;
					state.elements[1] = ColA;
					state.elements[2] = RowB;
					state.elements[3] = ColB;
					state.elements[4] = RowC;
					state.elements[5] = ColC;

				 	* Parametrization 2 Agent case:
					state.elements[0] = (i / number_of_columns * number_of_rows * number_of_columns) 	% number_of_rows;
					state.elements[1] = (i / number_of_columns * number_of_rows) 	% number_of_columns;
					state.elements[2] = (i / number_of_columns) 					% number_of_rows;
					state.elements[3] = (i / 1) 									% number_of_columns;
				 */

				unsigned currentAgentId = j / DIMENSION_FOR_AGENT;

				unsigned numberOfFieldsToPass = pow(number_of_columns * number_of_rows * dimension_fuel, (number_of_agents - currentAgentId - 1));

				state.elements[j] 	= (i / (numberOfFieldsToPass * number_of_columns * dimension_fuel) ) 	% number_of_rows;
				state.elements[j+1] = (i / (numberOfFieldsToPass * dimension_fuel))							% number_of_columns;
				state.elements[j+2] = (i / numberOfFieldsToPass) 											% dimension_fuel;
			}

			if(!Check_Blocked_State(state))
			{
				state.index = index++;
				states.push_back(state);
			}
		}


		/*
		vector<SmartVector> statesSingle; 	// for single agent.
		vector<SmartVector> statesMulti; 	// for multiagent

		int index=0;

		for (int r = 0; r < number_of_rows; r++) {
			for (int c = 0; c < number_of_columns; c++) {

				SmartVector state(2);
				state.elements[0] = r;
				state.elements[1] = c;

				if(!Gridworld::Check_Blocked_State(state))
				{
					state.index = index;
					statesSingle.push_back(state);
					index++;
				}
			}
		}

		// TODO: needs to be updated for parametric multiagent case.
		// Get The combination of states:
		index=0;
		for (unsigned i = 0; i < statesSingle.size(); ++i)
		{
			for (unsigned j = 0; j < statesSingle.size(); ++j)
			{
				SmartVector newState = SmartVector::Combine(statesSingle[i],statesSingle[j]);
				newState.index = index++;
				statesMulti.push_back(newState);
			}
		}

		states = statesMulti;
		*/
	}

	return states;
}

//TODO: needs to be updated for parametric multiagent case. -> FIXED
//TODO: It is currently just for single terminal state. Needs to be updated for
//multiple terminal states.
bool Rendezvous_Refuel::Check_Terminal_State(const SmartVector& state) const
{
	SmartVector terminalState;

	for (unsigned i = 0; i < number_of_agents; ++i)
	{
		terminalState = SmartVector::Combine(terminalState,terminal_states[0]);
	}

	if(state == terminalState)
	{
		return true;
	}


	return false;
}

//TODO: needs to be fixed for multiblock
//Fixed: needs to be fixed for multiagent case
bool Rendezvous_Refuel::Check_Blocked_State(const SmartVector& state) const
{
	vector<SmartVector> splittedStates = SmartVector::Split(state,number_of_agents);

	for (size_t i = 0; i < splittedStates.size(); ++i)
	{
		if(Gridworld_Refuel::Check_Blocked_State(splittedStates[i]))
		{
			return true;
		}
	}

	return false;
}

//TODO needs to be updated for parametric multiagent case.
vector<SmartVector> Rendezvous_Refuel::Get_Action_List(const SmartVector& state) const
{
	/*
	vector<SmartVector> actionsSingleAgent = Gridworld::Get_Action_List(state);
	vector<SmartVector> actionsMultiAgent;

	int index=0;
	for (unsigned i = 0; i < actionsSingleAgent.size(); ++i)
	{
		for (unsigned j = 0; j < actionsSingleAgent.size(); ++j)
		{
			SmartVector newAction = SmartVector::Combine(actionsSingleAgent[i],actionsSingleAgent[j]);
			newAction.index = index++;
			actionsMultiAgent.push_back(newAction);
		}
	}

	return actionsMultiAgent;
	*/

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

//FIXED: Fix for multiagent case. - FIXED
//TODO: Fix for multi terminal state case.
//TODO: Uncomment Distributed Reward for huge state spaces.
double Rendezvous_Refuel::Get_Reward(const SmartVector& currentState, const SmartVector& action, const SmartVector& nextState)
{
	double reward = 0.0;

	// Initialize terminal state for at least two agent:
	SmartVector terminalState;

	// If there are more than two agent, adapt terminal state by modifiying it.
	for (unsigned i = 0; i < number_of_agents; ++i)
	{
		terminalState = SmartVector::Combine(terminalState,terminal_states[0]);
	}

	/*
	 * Model 0 Naive
	 */
	/*
	if(nextState == terminalState)
	{
		reward = rewards_of_terminal_states[0];
	}
	*/


	/*
	 * Model 1 Multiple
	*/
	/*
	if(nextState == terminalState)
	{
		reward = rewards_of_terminal_states[0];
	}
	else
	{
		// Get the distance from state to terminal
		SmartVector diff = terminalState - nextState;
		double dist = diff.Magnitude();

		// Calculate reward relative to distance
		reward = 0.01 * 1.0 / (1.0 + dist);
	}
	*/


	/*
	 * Model 2 Single
	 */

	if(nextState == terminalState)
	{
		reward = rewards_of_terminal_states[0];
	}
	else
	{
		vector<SmartVector>  nextstates = SmartVector::Split(nextState,number_of_agents);
		//vector<SmartVector>  currentstates = SmartVector::Split(currentState,number_of_agents);

		double temp=0;
		for (unsigned i = 0; i < number_of_agents; ++i)
		{
			SmartVector diff_nextstate = terminal_states[0] - nextstates[i];
			//SmartVector diff_currstate = terminal_states[0] - currentstates[i];

			double dist_next = diff_nextstate.Magnitude();
			//double dist_curr = diff_currstate.Magnitude();

			temp += 1.0/(1.0+dist_next);
			/*
			if(dist_next<=dist_curr)
				temp += 1.0/(1.0+dist_next);
			else
				temp -= 100;
			*/
		}

		reward = 0.001*temp;
	}

	return reward+cost_action;
}

//FIXED: Fix to multiagent case for n>2
SmartVector Rendezvous_Refuel::Get_Initial_State()
{
	SmartVector vec;

	for (size_t i = 0; i < number_of_agents; ++i)
	{
		vec = SmartVector::Combine(vec,initial_state[i]);
	}

	vec.index = Get_State_Index(vec);

	return vec;
}

/*
SmartVector Rendezvous_Refuel::Get_Random_State()
{
	return Get_Initial_State();
}
*/
vector<pair<SmartVector,double>> Rendezvous_Refuel::Get_Transition_Probability(const SmartVector& currentState, const SmartVector& action)
{
	// Create Instance of return variable
	vector<pair<SmartVector,double>> state_probability_composed;

	vector< vector<pair<SmartVector,double>> > state_probability_decomposed;

	// Decompose states and actions
	vector<SmartVector> states = SmartVector::Split(currentState,number_of_agents);
	vector<SmartVector> actions = SmartVector::Split(action,number_of_agents);

	// Get the state transition probabilities of each agents
	for (unsigned i = 0; i < number_of_agents; ++i)
	{
		states[i].index = Gridworld_Refuel::Get_State_Index(states[i]);
		vector<pair<SmartVector,double>> prob = Gridworld_Refuel::Get_Transition_Probability(states[i],actions[i]);
		state_probability_decomposed.push_back(prob);
	}

	//TODO needs to be updated for parametric multiagent case.
	//Compose transition probabilities:
	for (unsigned i = 0; i < state_probability_decomposed[0].size(); ++i)
	{
		for (unsigned j = 0; j < state_probability_decomposed[1].size(); ++j)
		{
			SmartVector s = SmartVector::Combine(state_probability_decomposed[0][i].first,state_probability_decomposed[1][j].first);
			double prob = state_probability_decomposed[0][i].second * state_probability_decomposed[1][j].second;
			s.index = Get_State_Index(s);
			state_probability_composed.push_back(make_pair(s,prob));
		}
	}

	return state_probability_composed;
}

//TODO Use Hashmaps instead of bruteforce search
int Rendezvous_Refuel::Get_State_Index(const SmartVector& state) const
{
	vector<SmartVector> states = Rendezvous_Refuel::Get_All_Possible_States();

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


SmartVector Rendezvous_Refuel::Get_Next_State(const SmartVector& state, const SmartVector& action)
{
	// Decompose states and actions
	vector<SmartVector> states = SmartVector::Split(state,number_of_agents);
	vector<SmartVector> actions = SmartVector::Split(action,number_of_agents);

	// Create next states and fill it from super class.
	vector<SmartVector> nextStates;

	for (unsigned i = 0; i < number_of_agents; ++i) {
		states[i].index = Gridworld_Refuel::Get_State_Index(states[i]);

		SmartVector nextstate = Gridworld_Refuel::Get_Next_State(states[i],actions[i]);

		nextStates.push_back(nextstate);
	}

	SmartVector newStateComposed;
	for (unsigned i = 0; i < number_of_agents; ++i) {
		newStateComposed = SmartVector::Combine(newStateComposed,nextStates[i]);
	}

	newStateComposed.index = Get_State_Index(newStateComposed);
	return newStateComposed;
}

void Rendezvous_Refuel::Display_Policy(const Representation& rep)  const
{
	cout<<endl<<endl<<"NOT IMPLEMENTED YET"<<endl<<endl;
}

void Rendezvous_Refuel::Display_State(const SmartVector& state) const
{
	vector<SmartVector> agent_states = SmartVector::Split(state,number_of_agents);

	for (unsigned a = 0; a < number_of_agents; ++a) {

		cout<<"Displaying State for Agent #"<<a<<endl;

		Gridworld_Refuel::Display_State(agent_states[a]);

		cout<<endl<<endl;
	}
}
