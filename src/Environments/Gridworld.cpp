/*
 * gridworld.cpp
 *
 *  Created on: Mar 7, 2017
 *      Author: farabiahmed
 */

#include <Environments/Gridworld.hpp>

Gridworld::Gridworld(const ConfigParser& cfg) {

	//cout<<"Class Gridworld Constructor"<<endl;

	Name = "Grid World Environment";

	terminal_states = cfg.GetValueOfKey< vector<SmartVector> >("TERMINAL_STATES");

	blocked_states = cfg.GetValueOfKey< vector<SmartVector> >("BLOCKED_STATES");

	rewards_of_terminal_states = cfg.GetValueOfKey< vector<double> >("REWARDS");

	cost_action = cfg.GetValueOfKey<double>("COST_ACTION",-0.04);

	probability_of_random_action = cfg.GetValueOfKey<double>("PROBABILITY_OF_RANDOM_ACTION",0.20);

	number_of_columns = cfg.GetValueOfKey<int>("NUMBER_OF_COLUMNS");

	number_of_rows = cfg.GetValueOfKey<int>("NUMBER_OF_ROWS");

	number_of_states = number_of_columns * number_of_rows;

	number_of_actions = 4;
}

Gridworld::~Gridworld() {

	//cout<<"Class Gridworld Destructor"<<endl;
}

double Gridworld::Get_Reward(const SmartVector& currentState, const SmartVector& action, const SmartVector& nextState)
{
	double reward = 0.0;

	for (unsigned i = 0; i < terminal_states.size(); ++i)
	{
		if(nextState == terminal_states[i])
		{
			reward = rewards_of_terminal_states[i];
		}
	}

	return reward+cost_action;
}

vector<pair<SmartVector,double>> Gridworld::Get_Transition_Probability(const SmartVector& currentState, const SmartVector& action)
{
	// our model is a discrete probability transition function f_hat-> busoni,Algorithm 2.2

	//action 0: North
	//action 1: East
	//action 2: South
	//action 3: West

	double pDes = 1.0 - probability_of_random_action;
	double pLeft = probability_of_random_action*0.5;
	double pRight = probability_of_random_action*0.5;

	// Get all actions
	vector<SmartVector> actions = Get_Action_List(currentState);

	// Get all states
	vector<SmartVector> states = Get_All_Possible_States();

	// Create Instance of return variable
	vector<pair<SmartVector,double>> state_probability;

	// Temporary Pair that holds next possible state and its probability.
	pair<SmartVector,double> tempPair;

	// North
	if(action.elements[0]==0)
	{
		if ( Get_Feasibility_Of_Action(currentState,actions[0]) )
		{
			tempPair = make_pair(states[currentState.index-number_of_columns],pDes);

			state_probability.push_back(tempPair);
		}
		else
		{
			tempPair = make_pair(currentState,pDes);
			state_probability.push_back(tempPair);
		}

		Get_Feasibility_Of_Action(currentState,actions[1]) ?
				state_probability.push_back(make_pair(states[currentState.index+1],pRight)) :
				state_probability.push_back(make_pair(currentState,pRight));

		Get_Feasibility_Of_Action(currentState,actions[3]) ?
				state_probability.push_back(make_pair(states[currentState.index-1],pLeft)) :
				state_probability.push_back(make_pair(currentState,pLeft));
	}
	// East
	else if (action.elements[0]==1)
	{
		Get_Feasibility_Of_Action(currentState,actions[1]) ?
				state_probability.push_back(make_pair(states[currentState.index+1],pDes)) :
				state_probability.push_back(make_pair(currentState,pDes));

		Get_Feasibility_Of_Action(currentState,actions[2]) ?
				state_probability.push_back(make_pair(states[currentState.index+number_of_columns],pRight)) :
				state_probability.push_back(make_pair(currentState,pRight));

		Get_Feasibility_Of_Action(currentState,actions[0]) ?
				state_probability.push_back(make_pair(states[currentState.index-number_of_columns],pLeft)) :
				state_probability.push_back(make_pair(currentState,pLeft));
	}
	// South
	else if (action.elements[0]==2)
	{
		Get_Feasibility_Of_Action(currentState,actions[2]) ?
				state_probability.push_back(make_pair(states[currentState.index+number_of_columns],pDes)) :
				state_probability.push_back(make_pair(currentState,pDes));

		Get_Feasibility_Of_Action(currentState,actions[3]) ?
				state_probability.push_back(make_pair(states[currentState.index-1],pRight)) :
				state_probability.push_back(make_pair(currentState,pRight));

		Get_Feasibility_Of_Action(currentState,actions[1]) ?
				state_probability.push_back(make_pair(states[currentState.index+1],pLeft)) :
				state_probability.push_back(make_pair(currentState,pLeft));
	}
	// West
	else if (action.elements[0]==3)
	{
		Get_Feasibility_Of_Action(currentState,actions[3]) ?
				state_probability.push_back(make_pair(states[currentState.index-1],pDes)) :
				state_probability.push_back(make_pair(currentState,pDes));

		Get_Feasibility_Of_Action(currentState,actions[0]) ?
				state_probability.push_back(make_pair(states[currentState.index-number_of_columns],pRight)) :
				state_probability.push_back(make_pair(currentState,pRight));

		Get_Feasibility_Of_Action(currentState,actions[2]) ?
				state_probability.push_back(make_pair(states[currentState.index+number_of_columns],pLeft)) :
				state_probability.push_back(make_pair(currentState,pLeft));
	}
	return state_probability;
}

bool Gridworld::Get_Feasibility_Of_Action(const SmartVector& state, const SmartVector& action) const
{
	int r = state.elements[0];
	int c = state.elements[1];

	int r_blocked;
	int c_blocked;

	// Action North
	if(action.elements[0] == 0)
	{
		// can't go north if at top-most row 0 or if in the south of a blocked cell
		for (unsigned i = 0; i < blocked_states.size(); ++i) {

			r_blocked = blocked_states[i].elements[0];
			c_blocked = blocked_states[i].elements[1];

			if ((r==0) || (r==r_blocked+1 && c==c_blocked))
				return false;
		    return true;
		}
	}

	// Action East
	else if(action.elements[0] == 1)
	{
		// can't go east if at right-most col or if in the west of a blocked cell
		for (unsigned i = 0; i < blocked_states.size(); ++i) {

			r_blocked = blocked_states[i].elements[0];
			c_blocked = blocked_states[i].elements[1];

			if ((c==number_of_columns-1) || (r==r_blocked && c==c_blocked-1))
		        return false;
		    return true;
		}
	}

	// Action South
	else if(action.elements[0] == 2)
	{
		// can't go south if at bottom-most row or if in the north of a blocked cell
		for (unsigned i = 0; i < blocked_states.size(); ++i) {

			r_blocked = blocked_states[i].elements[0];
			c_blocked = blocked_states[i].elements[1];

			if ((r==number_of_rows-1) || (r==r_blocked-1 && c==c_blocked))
		        return false;
		    return true;
		}
	}

	// Action West
	else if(action.elements[0] == 3)
	{
		// can't go west if at left-most col or if in the east of a blocked cell
		for (unsigned i = 0; i < blocked_states.size(); ++i) {

			r_blocked = blocked_states[i].elements[0];
			c_blocked = blocked_states[i].elements[1];

			if ((c==0) || (r==r_blocked && c==c_blocked+1))
		        return false;
		    return true;
		}
	}


	return false;
}

vector<SmartVector> Gridworld::Get_All_Possible_States() const
{
	vector<SmartVector> states;

	int index=0;

	for (int r = 0; r < number_of_rows; r++) {
		for (int c = 0; c < number_of_columns; c++) {

			SmartVector state(2);
			state.elements[0] = r;
			state.elements[1] = c;
			state.index = index;
			states.push_back(state);
			index++;
		}
	}

	return states;
}

vector<SmartVector> Gridworld::Get_Action_List(const SmartVector& state) const
{
	vector<SmartVector> actions;

	SmartVector vec(1);

	// Add North (0)
	vec.elements[0] = 0;
	vec.index = 0;
	actions.push_back(vec);

	// Add East (1)
	vec.elements[0] = 1;
	vec.index = 1;
	actions.push_back(vec);

	// Add South (2)
	vec.elements[0] = 2;
	vec.index = 2;
	actions.push_back(vec);

	// Add West (3)
	vec.elements[0] = 3;
	vec.index = 3;
	actions.push_back(vec);

	return actions;
}


SmartVector Gridworld::Get_Next_State(const SmartVector& state, const SmartVector& action){

	SmartVector vec;

	vector<pair<SmartVector,double>> nextStateCandidates = Get_Transition_Probability(state,action);

	vector<double> probabilities;

	// Load probabilities to our list.
	for (unsigned int i = 0; i < nextStateCandidates.size(); ++i) {
		//cout<<nextState[i].first.index << " : "<< nextState[i].second<<endl;
		probabilities.push_back(nextStateCandidates[i].second);
	}

	// Create an instance of experiment engine.
	ProbabilityDistribution *p = new DiscreteDistribution(probabilities);

	// Get an sample from engine.
	vector<int> outcomes = p->Run_Experiment(1);

	// Check the candidate and return the state.
	for(unsigned int i = 0; i < outcomes.size(); i++)
	{
		if(outcomes[i]==1)
		{
			vec = nextStateCandidates[i].first;
			break;
		}
	}

	vec.index = Get_State_Index(vec);

	return vec;
}


SmartVector Gridworld::Get_Initial_State()
{
	SmartVector vec;

	vec = this->Get_All_Possible_States()[0];

	return vec;
}

bool Gridworld::Check_Terminal_State(const SmartVector& state)
{
	for (unsigned i = 0; i < terminal_states.size(); ++i)
	{
		if(state == terminal_states[i])
		{
			return true;
		}
	}

	return false;
}

bool Gridworld::Check_Blocked_State(const SmartVector& state) const
{
	for (unsigned i = 0; i < blocked_states.size(); ++i)
	{
		if(state == blocked_states[i])
		{
			return true;
		}
	}

	return false;
}

void Gridworld::Display_State(const SmartVector& state)  const
{

}

void Gridworld::Display_Action(const SmartVector& action) const
{

}

void Gridworld::Display_Policy(const Representation& rep)  const
{
	int Policy[number_of_rows][number_of_columns];
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
				Policy[i/number_of_columns][i%number_of_columns] = j;
			}
		}
	}

	for (int r = 0; r < number_of_rows; r++)
	{
		for (int c = 0; c < number_of_columns; c++)
		{
			char ch = (Policy[r][c]==0) ? '^' : (Policy[r][c]==1) ? '>' : (Policy[r][c]==2) ? '_' : '<';

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

			cout << "    ";
		}
		cout << " " << endl;
	}
}

int Gridworld::Get_State_Index(const SmartVector& state) const
{
	vector<SmartVector> states = Get_All_Possible_States();

	for (unsigned i = 0; i < states.size(); ++i) {
		if(state == states[i])
			return i;
	}

	cout<<endl<<endl<<"INVALID STATE!"<<endl<<endl;
	return -1;
}

void Gridworld::Test(void)
{
	cout<<"THIS IS TEST FUNCTION CALL:"<<endl;

	SmartVector state = Get_All_Possible_States()[10];
	SmartVector action = Get_Action_List(state)[0];

	for (int var = 0; var < 100; ++var) {
		SmartVector nextState = Get_Next_State(state,action);
		nextState.Print();
	}

}
