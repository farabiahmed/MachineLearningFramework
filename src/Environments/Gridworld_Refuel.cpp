/*
 * Gridworld_Refuel.cpp
 *
 *  Created on: Sep, 16, 2019
 *      Author: farabiahmed
 */

#include <Environments/Gridworld_Refuel.hpp>

Gridworld_Refuel::Gridworld_Refuel(const ConfigParser& cfg) {

	//cout<<"Class Gridworld_Refuel Constructor"<<endl;

	Name = "Grid World Refuel Environment";

	terminal_states = cfg.GetValueOfKey< vector<SmartVector> >("TERMINAL_STATES");

	blocked_states = cfg.GetValueOfKey< vector<SmartVector> >("BLOCKED_STATES");

	cout<<"Total Number Of Blocked States:" << blocked_states.size() << endl;

	refuel_states = cfg.GetValueOfKey< vector<SmartVector> >("REFUEL_STATES");

	rewards_of_terminal_states = cfg.GetValueOfKey< vector<double> >("REWARDS");

	cost_action = cfg.GetValueOfKey<double>("COST_ACTION",-0.04);

	probability_of_random_action = cfg.GetValueOfKey<double>("PROBABILITY_OF_RANDOM_ACTION",0.20);

	number_of_columns = cfg.GetValueOfKey<int>("NUMBER_OF_COLUMNS");

	number_of_rows = cfg.GetValueOfKey<int>("NUMBER_OF_ROWS");

	fuel_max = cfg.GetValueOfKey<int>("FUEL_MAX", 3);

	fuel_initial = cfg.GetValueOfKey<int>("FUEL_INITIAL", fuel_max);

	// Standardization scale first element of state (row).
	state_scalar.push_back( 1.0/(double)(number_of_rows-1)*2.0 );

	// Standardization scale second element of state (column).
	state_scalar.push_back( 1.0/(double)(number_of_columns-1)*2.0 );

	// Standardization mean first element of state (row).
	state_mean.push_back( (double)(number_of_rows-1)/2.0 );

	// Standardization mean second element of state (column).
	state_mean.push_back( (double)(number_of_columns-1)/2.0 );

	// Standardization scale second element of state (column).
	action_scalar.push_back( 1.0/1.5 );

	// Standardization mean first element of state (row).
	action_mean.push_back( 1.5);

	number_of_states = number_of_columns * number_of_rows;

	number_of_actions = 5;

}

Gridworld_Refuel::~Gridworld_Refuel() {

	//cout<<"Class Gridworld_Refuel Destructor"<<endl;
}

double Gridworld_Refuel::Get_Reward(const SmartVector& currentState, const SmartVector& action, const SmartVector& nextState)
{
	double reward = 0.0;


	for (unsigned i = 0; i < terminal_states.size(); ++i)
	{
		if(ComparePosition(nextState,terminal_states[i]))
		{
			reward = rewards_of_terminal_states[i];
		}
	}

	/*
	 * Distributed Reward through distance to terminal
	 */
	/*
	if(nextState == terminal_states[0])
	{
		reward = rewards_of_terminal_states[0];
	}
	else
	{
		// Get the terminal state for two agent
		SmartVector terminalState = terminal_states[0];

		// Get the distance from state to terminal
		SmartVector diff = terminalState - nextState;
		double dist = diff.Magnitude();

		// Calculate reward relative to distance
		reward = 0.01 * 1.0 / (1.0 + dist);
	}
	*/
	return reward+cost_action;
}

SmartVector Gridworld_Refuel::BurnFuel(const SmartVector &state)
{
	SmartVector temp = state;

	temp.elements[2]--;

	if(Check_Refuel_State(temp))
	{
		temp.elements[2] = fuel_max;
	}

	temp.index = Get_State_Index(temp);

	return temp;
}

vector<pair<SmartVector,double>> Gridworld_Refuel::Get_Transition_Probability(const SmartVector& currentState, const SmartVector& action)
{
	// our model is a discrete probability transition function f_hat-> busoni,Algorithm 2.2

	//action 0: North
	//action 1: East
	//action 2: South
	//action 3: West
	//action 4: Nothing

	double pDes = 1.0 - probability_of_random_action;
	double pLeft = probability_of_random_action*0.5;
	double pRight = probability_of_random_action*0.5;

	// Get all actions
	vector<SmartVector> actions = Gridworld_Refuel::Get_Action_List(currentState);

	// Get all states
	vector<SmartVector> states = Gridworld_Refuel::Get_All_Possible_States();

	// Create Instance of return variable
	vector<pair<SmartVector,double>> state_probability;

	// Temporary Pair that holds next possible state and its probability.
	pair<SmartVector,double> tempPair;

	// North
	if(action.elements[0]==0)
	{

		if ( Get_Feasibility_Of_Action(currentState,actions[0]) )
		{
			tempPair = make_pair(BurnFuel(states[currentState.index-number_of_columns*(fuel_max+1)]),pDes);
			state_probability.push_back(tempPair);
		}
		else
		{
			tempPair = make_pair(currentState,pDes);
			state_probability.push_back(tempPair);
		}

		/*
		Get_Feasibility_Of_Action(currentState,actions[0]) ?
				state_probability.push_back(make_pair(states[currentState.index-number_of_columns],pDes)) :
				state_probability.push_back(make_pair(currentState,pDes));
		*/
		Get_Feasibility_Of_Action(currentState,actions[1]) ?
				state_probability.push_back(make_pair(BurnFuel(states[currentState.index+1*(fuel_max+1)]),pRight)) :
				state_probability.push_back(make_pair(currentState,pRight));

		Get_Feasibility_Of_Action(currentState,actions[3]) ?
				state_probability.push_back(make_pair(BurnFuel(states[currentState.index-1*(fuel_max+1)]),pLeft)) :
				state_probability.push_back(make_pair(currentState,pLeft));

	}
	// East
	else if (action.elements[0]==1)
	{
		Get_Feasibility_Of_Action(currentState,actions[1]) ?
				state_probability.push_back(make_pair(BurnFuel(states[currentState.index+1*(fuel_max+1)]),pDes)) :
				state_probability.push_back(make_pair(currentState,pDes));

		Get_Feasibility_Of_Action(currentState,actions[2]) ?
				state_probability.push_back(make_pair(BurnFuel(states[currentState.index+number_of_columns*(fuel_max+1)]),pRight)) :
				state_probability.push_back(make_pair(currentState,pRight));

		Get_Feasibility_Of_Action(currentState,actions[0]) ?
				state_probability.push_back(make_pair(BurnFuel(states[currentState.index-number_of_columns*(fuel_max+1)]),pLeft)) :
				state_probability.push_back(make_pair(currentState,pLeft));
	}
	// South
	else if (action.elements[0]==2)
	{
		Get_Feasibility_Of_Action(currentState,actions[2]) ?
				state_probability.push_back(make_pair(BurnFuel(states[currentState.index+number_of_columns*(fuel_max+1)]),pDes)) :
				state_probability.push_back(make_pair(currentState,pDes));

		Get_Feasibility_Of_Action(currentState,actions[3]) ?
				state_probability.push_back(make_pair(BurnFuel(states[currentState.index-1*(fuel_max+1)]),pRight)) :
				state_probability.push_back(make_pair(currentState,pRight));

		Get_Feasibility_Of_Action(currentState,actions[1]) ?
				state_probability.push_back(make_pair(BurnFuel(states[currentState.index+1*(fuel_max+1)]),pLeft)) :
				state_probability.push_back(make_pair(currentState,pLeft));
	}
	// West
	else if (action.elements[0]==3)
	{
		Get_Feasibility_Of_Action(currentState,actions[3]) ?
				state_probability.push_back(make_pair(BurnFuel(states[currentState.index-1*(fuel_max+1)]),pDes)) :
				state_probability.push_back(make_pair(currentState,pDes));

		Get_Feasibility_Of_Action(currentState,actions[0]) ?
				state_probability.push_back(make_pair(BurnFuel(states[currentState.index-number_of_columns*(fuel_max+1)]),pRight)) :
				state_probability.push_back(make_pair(currentState,pRight));

		Get_Feasibility_Of_Action(currentState,actions[2]) ?
				state_probability.push_back(make_pair(BurnFuel(states[currentState.index+number_of_columns*(fuel_max+1)]),pLeft)) :
				state_probability.push_back(make_pair(currentState,pLeft));
	}
	// Do Nothing
	else if (action.elements[0]==4)
	{
		state_probability.push_back(make_pair(currentState,1.0));
	}

	// Get the index of returned state
	for (unsigned i = 0; i < state_probability.size(); ++i) {
		state_probability[i].first.index = Gridworld_Refuel::Get_State_Index(state_probability[i].first);
	}

	return state_probability;
}

bool Gridworld_Refuel::Get_Feasibility_Of_Action(const SmartVector& state, const SmartVector& action) const
{
	int r = state.elements[0];
	int c = state.elements[1];
	int f = state.elements[2]; //fuel

	int r_blocked;
	int c_blocked;

	if(f==0) return false; //if no fuel, you cannot move.

	// Action North
	if(action.elements[0] == 0)
	{
		// can't go north if at top-most row 0 or if in the south of a blocked cell
		for (unsigned i = 0; i < blocked_states.size(); ++i) {

			r_blocked = blocked_states[i].elements[0];
			c_blocked = blocked_states[i].elements[1];

			if ((r==0) || (r==r_blocked+1 && c==c_blocked))
				return false;
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
		}
	}


	return true;
}

vector<SmartVector> Gridworld_Refuel::Get_All_Possible_States() const
{
	vector<SmartVector> states;

	int index=0;

	for (int r = 0; r < number_of_rows; r++) {
		for (int c = 0; c < number_of_columns; c++) {
			for (int f = fuel_max; f >= 0; f--) {

				SmartVector state(3);
				state.elements[0] = r;
				state.elements[1] = c;
				state.elements[2] = f;
				state.index = index;
				states.push_back(state);
				index++;
			}
		}
	}

	return states;
}

vector<SmartVector> Gridworld_Refuel::Get_Action_List(const SmartVector& state) const
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

	// Add DoNothing (4)
	vec.elements[0] = 4;
	vec.index = 4;
	actions.push_back(vec);

	return actions;
}


SmartVector Gridworld_Refuel::Get_Next_State(const SmartVector& state, const SmartVector& action){

	SmartVector vec;

	vector<pair<SmartVector,double>> nextStateCandidates = Gridworld_Refuel::Get_Transition_Probability(state,action);

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

	//vec.index = Get_State_Index(vec);

	delete p;
	return vec;
}


SmartVector Gridworld_Refuel::Get_Initial_State()
{
	SmartVector vec;

	vec = this->Get_All_Possible_States()[0];

	return vec;
}

SmartVector Gridworld_Refuel::Get_Random_State()
{
	static SmartVector state;

	state = Environment::Get_Random_State();

	state.elements[2] = fuel_max;
	/*
	while(state.elements[StateIdx::Fuel] != fuel_max)
	{
		state = Environment::Get_Random_State();
	}
	*/
	return state;
}

bool Gridworld_Refuel::ComparePosition(const SmartVector& state1, const SmartVector& state2) const
{
	if( (state1.elements[0]==state2.elements[0]) && (state1.elements[1]==state2.elements[1]) )
	{
		return true;
	}

	return false;
}

bool Gridworld_Refuel::Check_Terminal_State(const SmartVector& state) const
{
	for (unsigned i = 0; i < terminal_states.size(); ++i)
	{
		if(ComparePosition(state,terminal_states[i]))
		{
			return true;
		}
	}

	return false;
}

bool Gridworld_Refuel::Check_Blocked_State(const SmartVector& state) const
{
	for (unsigned i = 0; i < blocked_states.size(); ++i)
	{
		if(ComparePosition(state,blocked_states[i]))
		{
			return true;
		}
	}

	return false;
}

bool Gridworld_Refuel::Check_Refuel_State(const SmartVector& state) const
{
	for (unsigned i = 0; i < refuel_states.size(); ++i)
	{
		if(ComparePosition(state,refuel_states[i]))
		{
			return true;
		}
	}

	return false;
}

void Gridworld_Refuel::Display_State(const SmartVector& agent)  const
{
//	cout<<"State "<<state.index<<": ";
//	for (int i = 0; i < state.size(); ++i) {
//		cout<<state.elements[i]<<" ";
//	}
//	cout<<endl;

	// For more unicode characters: http://shapecatcher.com/

	// For Fuel Station
	// Filled Hexagon: 2B22
	// Empty Hexagon: 2B21

	// Blocked State
	// Cross: 2612

	// Positive Reward:
	// Tick 2714
	// InBlock Tick 2611

	// Negative Reward
	// Skull 2620

	// Ordinary State
	// Occupied 25A0
	// Empty 25A1

	// Future Use For MultiAgent:
	// Numbered items: 2781 and 278B

	cout<<"Displaying Current Status:"<<endl;

	//for (int r = number_of_rows-1; r >= 0; r--)
	for (int r = 0; r < number_of_rows; r++)
	{
		for (int c = 0; c < number_of_columns; c++)
		{
			SmartVector state(2);
			state.elements[0] = r;
			state.elements[1] = c;

			for (unsigned i = 0; i < blocked_states.size(); ++i)
			{
				if(ComparePosition(state, blocked_states[i]))
				{
					cout<<"\u2612";
					goto position_placed;
				}
			}

			for (unsigned i = 0; i < terminal_states.size(); ++i)
			{
				if(ComparePosition(state, terminal_states[i]))
				{
					if(rewards_of_terminal_states[i]>0)
					{
						if(ComparePosition(agent, state))
						{
							cout<<"\033[32m\u2611\033[0m"; // Green Square Tick
							goto position_placed;
						}
						else
						{
							cout<<"\u2714"; // Tick
							goto position_placed;
						}
					}
					else if(rewards_of_terminal_states[i]<0)
					{
						if(ComparePosition(agent, state))
						{
							cout<<"\033[31m\u2620\033[0m"; // Red Skull with bones
							goto position_placed;
						}
						else
						{
							cout<<"\u2620"; // Skull with bones
							goto position_placed;
						}
					}
				}
			}

			for (unsigned i = 0; i < refuel_states.size(); ++i)
			{
				if(ComparePosition(state, refuel_states[i]))
				{
					if(ComparePosition(agent, state))
					{
						cout<<"\033[32m\u2B22\033[0m"; // Green Hexagon Filled
						goto position_placed;
					}
					else
					{
						cout<<"\u2B21"; // Hexagon Empty
						goto position_placed;
					}
				}
			}

			if(ComparePosition(agent, state))
			{
				cout<<"\033[32m\u25A0\033[0m"; // Green Hexagon Filled
				goto position_placed;
			}
			else
			{
				cout<<"\u25A1"; // Hexagon Empty
				goto position_placed;
			}

position_placed:
			cout << " ";
		}
		cout << " " << endl;
	}

	cout<<"Current Fuel: "<<agent.elements[2]<<endl;
}

void Gridworld_Refuel::Display_Action(const SmartVector& action) const
{
	action.Print();
}

void Gridworld_Refuel::Display_Policy(const Representation& rep)  const
{
	cout<<endl<<endl<<"Displaying Policy Status:"<<endl;

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
			char ch = (Policy[r][c]==4) ? 'o' : (Policy[r][c]==0) ? '^' : (Policy[r][c]==1) ? '>' : (Policy[r][c]==2) ? '_' : '<';

			SmartVector state(2);
			state.elements[0] = r;
			state.elements[1] = c;

			for (unsigned i = 0; i < blocked_states.size(); ++i) {
				if(ComparePosition(state, blocked_states[i]))
					ch = 'x';
			}

			for (unsigned i = 0; i < terminal_states.size(); ++i) {
				if(ComparePosition(state, terminal_states[i]))
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
}

//TODO Use Hashmaps instead of bruteforce search
int Gridworld_Refuel::Get_State_Index(const SmartVector& state) const
{
	vector<SmartVector> states = Gridworld_Refuel::Get_All_Possible_States();

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

void Gridworld_Refuel::Test(void)
{
	cout<<"THIS IS TEST FUNCTION CALL:"<<endl;

	SmartVector state = Get_All_Possible_States()[10];
	SmartVector action = Get_Action_List(state)[0];

	for (int var = 0; var < 100; ++var) {
		SmartVector nextState = Get_Next_State(state,action);
		nextState.Print();
	}

}
