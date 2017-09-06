/*
 * Rendezvous.cpp
 *
 *  Created on: Jul 26, 2017
 *      Author: farabiahmed
 */

#include <Environments/Rendezvous.hpp>

Rendezvous::Rendezvous(const ConfigParser& cfg)
:Gridworld(cfg)
{
	number_of_agents = cfg.GetValueOfKey<int>("NUMBER_OF_AGENTS");

	initial_state = cfg.GetValueOfKey<vector<SmartVector>>("INITIAL_STATE");
}

Rendezvous::~Rendezvous()
{

}


vector<SmartVector> Rendezvous::Get_All_Possible_States() const
{
	static vector<SmartVector> states;		//Cache states since it is expensive to calculate

	if(states.size()==0)
	{
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
	}

	return states;
}

//TODO needs to be updated for parametric multiagent case.
//It is currently just for single terminal state. Needs to be updated for
//multiple terminal states.
bool Rendezvous::Check_Terminal_State(const SmartVector& state) const
{
	for (unsigned i = 0; i < terminal_states.size(); ++i)
	{
		if(state == SmartVector::Combine(terminal_states[i],terminal_states[i]))
		{
			return true;
		}
	}

	return false;
}

//TODO needs to be updated for parametric multiagent case.
bool Rendezvous::Check_Blocked_State(const SmartVector& state) const
{
	/*
	for (unsigned i = 0; i < blocked_states.size(); ++i)
	{
		if(state == SmartVector::Combine(blocked_states[i],blocked_states[i]))
		{
			return true;
		}
	}
	*/

	return false;
}

//TODO needs to be updated for parametric multiagent case.
vector<SmartVector> Rendezvous::Get_Action_List(const SmartVector& state) const
{
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
}

//TODO: Fix for multiagent case
double Rendezvous::Get_Reward(const SmartVector& currentState, const SmartVector& action, const SmartVector& nextState)
{
	double reward = 0.0;
	/*
	for (unsigned i = 0; i < terminal_states.size(); ++i)
	{
		if(nextState == SmartVector::Combine(terminal_states[i],terminal_states[i]))
		{
			reward = rewards_of_terminal_states[i];
		}
	}
	*/

	if(nextState == SmartVector::Combine(terminal_states[0],terminal_states[0]))
	{
		reward = rewards_of_terminal_states[0];
	}
	else
	{
		// Get the terminal state for two agent
		SmartVector terminalState = SmartVector::Combine(terminal_states[0],terminal_states[0]);

		// Get the distance from state to terminal
		SmartVector diff = terminalState - nextState;
		double dist = diff.Magnitude();

		// Calculate reward relative to distance
		reward = 0.1 * 1.0 / (1.0 + dist);
	}

	/*
	vector<SmartVector> states = SmartVector::Split(nextState,number_of_agents);
	SmartVector diff1 = terminal_states[0] - states[0];
	SmartVector diff2 = terminal_states[0] - states[1];

	double dist1 = diff1.Magnitude();
	double dist2 = diff2.Magnitude();

	reward = 1.0 / (1.0 + dist1 * dist2);
	*/

	return reward+cost_action;
}

//TODO: Fix to multiagent case for n>2
SmartVector Rendezvous::Get_Initial_State()
{
	SmartVector vec;

	//vec = this->Get_All_Possible_States()[0];

	vec = SmartVector::Combine(initial_state[0],initial_state[1]);

	vec.index = Get_State_Index(vec);

	return vec;
}

SmartVector Rendezvous::Get_Random_State()
{
	return Get_Initial_State();
}

vector<pair<SmartVector,double>> Rendezvous::Get_Transition_Probability(const SmartVector& currentState, const SmartVector& action)
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
		states[i].index = Gridworld::Get_State_Index(states[i]);
		vector<pair<SmartVector,double>> prob = Gridworld::Get_Transition_Probability(states[i],actions[i]);
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
int Rendezvous::Get_State_Index(const SmartVector& state) const
{
	vector<SmartVector> states = Rendezvous::Get_All_Possible_States();

	for (unsigned i = 0; i < states.size(); ++i) {
		if(state == states[i])
			return i;
	}

	cout<<endl<<endl<<"INVALID STATE!"<<endl<<endl;

	cout<<"Controlled Abort..."<<endl;
	abort();
	return -1;
}


SmartVector Rendezvous::Get_Next_State(const SmartVector& state, const SmartVector& action)
{
	// Decompose states and actions
	vector<SmartVector> states = SmartVector::Split(state,number_of_agents);
	vector<SmartVector> actions = SmartVector::Split(action,number_of_agents);

	// Create next states and fill it from super class.
	vector<SmartVector> nextStates;

	for (unsigned i = 0; i < number_of_agents; ++i) {
		states[i].index = Gridworld::Get_State_Index(states[i]);
		nextStates.push_back(Gridworld::Get_Next_State(states[i],actions[i]));
	}

	SmartVector newStateComposed;
	for (unsigned i = 0; i < number_of_agents-1; ++i) {
		newStateComposed = SmartVector::Combine(nextStates[i],nextStates[i+1]);
	}

	newStateComposed.index = Get_State_Index(newStateComposed);
	return newStateComposed;
}

void Rendezvous::Display_Policy(const Representation& rep)  const
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



	for (unsigned a = 0; a < number_of_agents; ++a) {

		cout<<"Agent #"<<a<<":"<<endl;

		for (int r = 0; r < number_of_rows; r++)
		{
			for (int c = 0; c < number_of_columns; c++)
			{
				unsigned policy = Policy[a][r][c];

				char ch = (policy==4) ? 'o' : (policy==0) ? '^' : (policy==1) ? '>' : (policy==2) ? '_' : '<';

				SmartVector state(2);
				state.elements[0] = r;
				state.elements[1] = c;

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
				else
					cout<<ch;

				cout << " ";
			}
			cout << " " << endl;
		}
		cout << endl;
	}
}
