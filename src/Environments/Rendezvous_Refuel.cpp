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
}

Rendezvous_Refuel::~Rendezvous_Refuel()
{

}


vector<SmartVector> Rendezvous_Refuel::Get_All_Possible_States() const
{
	static vector<SmartVector> states;		//Cache states since it is expensive to calculate

	int index=0;

	if(states.size()==0)
	{
		size_t size_of_states = pow(number_of_rows * number_of_columns, number_of_agents);

		for (size_t i = 0; i < size_of_states; ++i)
		{
			SmartVector state(2 * number_of_agents);

			for (int j = 0; j < state.size(); j+=2) {

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

				unsigned currentAgentId = j / 2;

				unsigned numberOfFieldsToPass = pow(number_of_columns * number_of_rows, (number_of_agents - currentAgentId - 1));

				state.elements[j] 	= (i / (numberOfFieldsToPass * number_of_columns) ) 	% number_of_rows;
				state.elements[j+1] = (i / numberOfFieldsToPass) 						% number_of_columns;
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

//TODO needs to be fixed for multiblock and multiagent case -> multiagent-FIXED
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

//TODO: Fix for multiagent case. - FIXED
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

//TODO: Fix to multiagent case for n>2
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
	cout<<endl<<endl<<"Displaying Policy Status:"<<endl<<endl;

	int Policy[number_of_agents][number_of_rows][number_of_columns];
	double maxQValue=0;

	vector<SmartVector> states = Get_All_Possible_States();
	vector<SmartVector> actions;

	for (unsigned i = 0; i < states.size(); ++i) {

		maxQValue = std::numeric_limits<double>::lowest();

		actions = Get_Action_List(states[i]);

		for (unsigned j = 0; j < actions.size(); ++j) {

			double temp = rep.Get_Value(states[i],actions[j]);
			if(temp>maxQValue)
			{
				//maxQValue = rep.Qvalue[i][j];
				maxQValue = temp;

				for (unsigned agent = 0; agent < number_of_agents; ++agent)
				{
					int policy = actions[j].elements[agent];
					unsigned r = (unsigned) states[i].elements[agent*2 + 0];
					unsigned c = (unsigned) states[i].elements[agent*2 + 1];

					Policy[agent][r][c] = policy;
				}
			}
		}
	}

}

void Rendezvous_Refuel::Display_State(const SmartVector& state) const
{
	char ch;

	vector<SmartVector> agent_states = SmartVector::Split(state,number_of_agents);

	for (int r = 0; r < number_of_rows; r++)
	{
		for (int c = 0; c < number_of_columns; c++)
		{
			ch = ' ';
			SmartVector state(2);
			state.elements[0] = r;
			state.elements[1] = c;

			for (unsigned a = 0; a < number_of_agents; ++a) {
				if(state==agent_states[a])
					ch = 'A' + a;
			}


			for (unsigned i = 0; i < blocked_states.size(); ++i) {
				if(state == blocked_states[i])
					ch = 'x';
			}

			for (unsigned i = 0; i < terminal_states.size(); ++i) {
				if(state == terminal_states[i])
				{
					if(rewards_of_terminal_states[i]>0)
						ch = '+';
					else if(rewards_of_terminal_states[i]<0)
						ch = '-';
				}
			}

			// For more unicode characters: http://shapecatcher.com/
			if(ch=='_')
				cout<<"\u2193";
			else if(ch=='<')
				cout<<"\u2190";
			else if(ch=='^')
				cout<<"\u2191";
			else if(ch=='>')
				cout<<"\u2192";
			else if(ch=='x')
				cout<<"\033[33m\u2612\033[0m"; // Yellow Blocked Cross
			else if(ch=='+')
				cout<<"\033[32m\u2714\033[0m"; // Green Tick
			else if(ch=='-')
				cout<<"\033[31m\u2620\033[0m"; // Red Skull with bones
			else if(ch==' ')
				cout<<"\u2317";
			else
				cout<<ch;

			cout << " ";
		}
		cout << " " << endl;
	}
}
