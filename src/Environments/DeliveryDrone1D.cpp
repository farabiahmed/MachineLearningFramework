/*
 * DeliveryDrone.cpp
 *
 *  Created on: Sep, 22, 2019
 *      Author: farabiahmed
 */

#include <Environments/DeliveryDrone1D.hpp>

using namespace std;

DeliveryDrone1D::DeliveryDrone1D(const ConfigParser& cfg) {

	//cout<<"Class DeliveryDrone Constructor"<<endl;

	Name = "DeliveryDrone1D Refuel Environment";
	cout <<Name<<endl;

	//terminal_states = cfg.GetValueOfKey< vector<SmartVector> >("TERMINAL_STATES");

	initial_state = cfg.GetValueOfKey<SmartVector>("INITIAL_STATE");

	blocked_states = cfg.GetValueOfKey< vector<SmartVector> >("BLOCKED_STATES");

	cout<<"Total Number Of Blocked States:" << blocked_states.size() << endl;

	refuel_states = cfg.GetValueOfKey< vector<SmartVector> >("REFUEL_STATES");

	rewards_of_terminal_states = cfg.GetValueOfKey< vector<double> >("REWARDS");

	cost_action = cfg.GetValueOfKey<double>("COST_ACTION",-0.04);

	probability_of_random_action = cfg.GetValueOfKey<double>("PROBABILITY_OF_RANDOM_ACTION",0);

	number_of_positions = cfg.GetValueOfKey<int>("NUMBER_OF_POSITIONS", 10);

	fuel_max = cfg.GetValueOfKey<int>("FUEL_MAX", number_of_positions/2);

	fuel_initial = cfg.GetValueOfKey<int>("FUEL_INITIAL", fuel_max);

/*
	// Standardization scale first element of state (row).
	state_scalar.push_back( 1.0/(double)(number_of_positions-1)*2.0 );

	// Standardization scale second element of state (column).
	state_scalar.push_back( 1.0/(double)(number_of_columns-1)*2.0 );

	// Standardization mean first element of state (row).
	state_mean.push_back( (double)(number_of_positions-1)/2.0 );

	// Standardization mean second element of state (column).
	state_mean.push_back( (double)(number_of_columns-1)/2.0 );

	// Standardization scale second element of state (column).
	action_scalar.push_back( 1.0/1.5 );

	// Standardization mean first element of state (row).
	action_mean.push_back( 1.5);
*/
	number_of_actions = 3;

	Get_All_Possible_States();

	if(initial_state.dimension>0 && initial_state.index==-1) 
		initial_state.index = Get_State_Index(initial_state);
	else
		initial_state = Get_Random_State();
		
	initial_state.Print();

	representation_model = cfg.GetValueOfKey<string>("REPRESENTATION");
}

DeliveryDrone1D::~DeliveryDrone1D() {

	//cout<<"Class DeliveryDrone Destructor"<<endl;
}

double DeliveryDrone1D::Get_Reward(const SmartVector& currentState, const SmartVector& action, const SmartVector& nextState)
{
	double reward = 0.0;

	if(Check_Packet_Picked(nextState) && !Check_Packet_Picked(currentState))
	{
		reward += 0.2;
	}

	if(Check_Terminal_State(nextState) && !Check_Terminal_State(currentState))
	{
		reward += 1;
		reward += currentState.elements[StateIdx::Fuel] * cost_action * -1;
	}

	if(action.elements[0]!=ActionValue::Wait)
		reward+=cost_action;

//	if(nextState.elements[StateIdx::Fuel] == 0) reward += -1;

/*
	for (unsigned i = 0; i < terminal_states.size(); ++i)
	{
		if(ComparePosition(nextState,terminal_states[i]))
		{
			reward = rewards_of_terminal_states[i];
		}
	}
*/
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
	return reward;
}

SmartVector DeliveryDrone1D::BurnFuel(const SmartVector &state)
{
	SmartVector temp = state;

	temp.elements[StateIdx::Fuel]--;

	if(Check_Refuel_State(temp))
	{
		temp.elements[StateIdx::Fuel] = fuel_max;
	}

	return temp;
}

vector<pair<SmartVector,double>> DeliveryDrone1D::Get_Transition_Probability(const SmartVector& currentState, const SmartVector& action)
{
	// our model is a discrete probability transition function f_hat-> busoni,Algorithm 2.2
#define DOUBLE_EPSILON 0.0005
	//action 0: North
	//action 1: East
	//action 2: South
	//action 3: West
	//action 4: Nothing

	double probabilityTrueAction = 1.0 - probability_of_random_action;
	double probabilityWait = probability_of_random_action*0.5;
	double probabilityWrongAction = probability_of_random_action*0.5;

	// Get all actions
	vector<SmartVector> actions = DeliveryDrone1D::Get_Action_List(currentState);

	// Get all states
	//vector<SmartVector> states = DeliveryDrone::Get_All_Possible_States();

	// Create Instance of return variable
	vector<pair<SmartVector,double>> state_probability;

	// Temporary Pair that holds next possible state and its probability.
	pair<SmartVector,double> tempPair;

	// State Candidate
	SmartVector stateCandid(StateIdx::TotalNumberOfStates);
	
	// East
	if (action.elements[0]==ActionValue::East)
	{
		Get_Feasibility_Of_Action(currentState,actions[ActionIndex::East], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),probabilityTrueAction)) :
				state_probability.push_back(make_pair(currentState,probabilityTrueAction));

		if(probabilityWrongAction>DOUBLE_EPSILON)
		Get_Feasibility_Of_Action(currentState,actions[ActionIndex::West], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),probabilityWrongAction)) :
				state_probability.push_back(make_pair(currentState,probabilityWrongAction));

		if(probabilityWait>DOUBLE_EPSILON)
		Get_Feasibility_Of_Action(currentState,actions[ActionIndex::Wait], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),probabilityWait)) :
				state_probability.push_back(make_pair(currentState,probabilityWait));
	}
	
	// West
	else if (action.elements[0]==ActionValue::West)
	{
		Get_Feasibility_Of_Action(currentState,actions[ActionIndex::West], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),probabilityTrueAction)) :
				state_probability.push_back(make_pair(currentState,probabilityTrueAction));

		if(probabilityWrongAction>DOUBLE_EPSILON)
		Get_Feasibility_Of_Action(currentState,actions[ActionIndex::East], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),probabilityWrongAction)) :
				state_probability.push_back(make_pair(currentState,probabilityWrongAction));

		if(probabilityWait>DOUBLE_EPSILON)
		Get_Feasibility_Of_Action(currentState,actions[ActionIndex::Wait], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),probabilityWait)) :
				state_probability.push_back(make_pair(currentState,probabilityWait));
	}
	// Do Nothing
	else // else if(action.elements[0]==ActionValue::Wait)
	{
		state_probability.push_back(make_pair(currentState,1.0));
	}

	// Get the index of returned state
	for (unsigned i = 0; i < state_probability.size(); ++i) {

		// If the new state is on packet or deliverystation, update the state accordingly.
		if(Check_Packet_Picked(state_probability[i].first) && Check_Above_DeliveryPoint(state_probability[i].first))
		{
			state_probability[i].first.elements[StateIdx::Position_DeliveryPoint] = -1;
		}
		else if(Check_Above_Packet(state_probability[i].first))
		{
			state_probability[i].first.elements[StateIdx::Position_Packet] = -1;
		}

		state_probability[i].first.index = DeliveryDrone1D::Get_State_Index(state_probability[i].first);
	}

	return state_probability;
}

bool DeliveryDrone1D::Get_Feasibility_Of_Action(const SmartVector& state, const SmartVector& action, SmartVector& statecandidate) const
{
	statecandidate = state;

	int pos = state.elements[StateIdx::Position_Agent];
	int f = state.elements[StateIdx::Fuel]; //fuel

	int pos_blocked;

	if(f==0) return false; //if no fuel, you cannot move.

	// 3 -> Left (A) West
	// 1 -> Right (D) East
	// Action East
	if(action.elements[0] == 1)
	{
		// can't go east if at right-most col or if in the west of a blocked cell
		for (unsigned i = 0; i < blocked_states.size(); ++i) {

			pos_blocked = blocked_states[i].elements[0];

			if (pos==pos_blocked-1)
		        return false;
		}

		if (pos==number_of_positions-1)
		{
			return false;
		} 

		statecandidate.elements[StateIdx::Position_Agent]++;
	}

	// Action West
	else if(action.elements[0] == 3)
	{
		// can't go west if at left-most col or if in the east of a blocked cell
		for (unsigned i = 0; i < blocked_states.size(); ++i) {

			pos_blocked = blocked_states[i].elements[0];

			if (pos==pos_blocked+1)
		        return false;
		}

		if (pos==0)
		{
			return false;
		} 

		statecandidate.elements[StateIdx::Position_Agent]--;
	}

	// DoNothing West
	else if(action.elements[0] == 3)
	{
		return true;
	}

	// Invalid actions
	else
	{
		return false;
	}
	
	return true;
}

vector<SmartVector>& DeliveryDrone1D::Get_All_Possible_States() const
{
	static vector<SmartVector> states;

	if(states.size()==0)
	{
		int index=0;

		cout << "number_of_positions "<< number_of_positions << endl;
		cout << "fuel_max "<< fuel_max << endl;

		for (int pos = 0; pos < number_of_positions; pos++) {
			for (int f = fuel_max; f >= 0; f--) {
				for (int pos_packet = -1; pos_packet < number_of_positions; pos_packet++) { //-1 means packet picked
					for (int pos_deliverypoint = -1; pos_deliverypoint < number_of_positions; pos_deliverypoint++) { // -1 means packet delivered

						SmartVector state(StateIdx::TotalNumberOfStates);

						state.elements[StateIdx::Position_Agent] = pos;

						state.elements[StateIdx::Fuel] = f;

						state.elements[StateIdx::Position_Packet] = pos_packet;

						state.elements[StateIdx::Position_DeliveryPoint] = pos_deliverypoint;

						// If there is packet but no delivery station, skip it.
						if(pos_packet>=0 && (pos_deliverypoint==-1))
							continue;

						// Check Packet and Delivery Point overlapped
						if(pos_packet == pos_deliverypoint && pos_packet>-1)
							continue;

						// Check whether the position on the blocked state
						bool invalid = false;
						for (size_t i = 0; i < blocked_states.size(); ++i)
						{
							if( pos == blocked_states[i].elements[0])
							{
								invalid = true;
								break;
							}

							if( pos_packet == blocked_states[i].elements[0])
							{
								invalid = true;
								break;
							}

							if( pos_deliverypoint == blocked_states[i].elements[0])
							{
								invalid = true;
								break;
							}
						}
						if(invalid) continue;

						for (size_t i = 0; i < refuel_states.size(); ++i)
						{
							if( pos_packet == refuel_states[i].elements[0])
							{
								invalid = true;
								break;
							}

							if( pos_deliverypoint == refuel_states[i].elements[0])
							{
								invalid = true;
								break;
							}
						}
						if(invalid) continue;


						state_indexes.emplace(state.Serialize(), index);

						state.index = index;
						states.push_back(state);
						index++;
					}								
				}
			}
		}

		cout<<"Total number of states: "<< states.size() << endl;
	}
	
	return states;
}

vector<SmartVector>& DeliveryDrone1D::Get_Action_List(const SmartVector& state) const
{
	static vector<SmartVector> actions;

	if(actions.size()==0)
	{
		SmartVector vec(1);

		/*
		// Add North (0)
		vec.elements[0] = 0;
		vec.index = 0;
		actions.push_back(vec);
		*/

		// Add East (1)
		vec.elements[0] = ActionValue::East;
		vec.index = ActionIndex::East;
		actions.push_back(vec);

		/*
		// Add South (2)
		vec.elements[0] = 2;
		vec.index = 2;
		actions.push_back(vec);
		*/

		// Add West (3)
		vec.elements[0] = ActionValue::West;
		vec.index = ActionIndex::West;
		actions.push_back(vec);

		// Add DoNothing (4)
		vec.elements[0] = ActionValue::Wait;
		vec.index = ActionIndex::Wait;
		actions.push_back(vec);
	}

	return actions;
}


SmartVector DeliveryDrone1D::Get_Next_State(const SmartVector& state, const SmartVector& action){

	SmartVector vec;

	vector<pair<SmartVector,double>> nextStateCandidates = DeliveryDrone1D::Get_Transition_Probability(state,action);

	vector<double> probabilities;

	// Load probabilities to our list.
	for (unsigned int i = 0; i < nextStateCandidates.size(); ++i) {
		//cout<<nextState[i].first.index << " : "<< nextState[i].second<<endl;
		probabilities.push_back(nextStateCandidates[i].second);
	}

	// Create an instance of experiment engine.
	ProbabilityDistribution *pos = new DiscreteDistribution(probabilities);

	// Get an sample from engine.
	vector<int> outcomes = pos->Run_Experiment(1);

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

	delete pos;
	return vec;
}


SmartVector DeliveryDrone1D::Get_Initial_State()
{
	SmartVector vec;

	vec = this->Get_All_Possible_States()[0];

	return vec;
}

SmartVector DeliveryDrone1D::Get_Random_State()
{
	if(initial_state.dimension>0)
	{
		return initial_state;
	}
	else
	{
		static SmartVector state;

		state = Environment::Get_Random_State();

		state.elements[StateIdx::Fuel] = fuel_max;
		/*
		while(state.elements[StateIdx::Fuel] != fuel_max)
		{
			state = Environment::Get_Random_State();
		}
		*/
		return state;
	}
}

bool DeliveryDrone1D::ComparePosition(const SmartVector& state1, const SmartVector& state2) const
{
	if( (state1.elements[0]==state2.elements[0]) )
	{
		return true;
	}

	return false;
}

bool DeliveryDrone1D::Check_Terminal_State(const SmartVector& state) const
{
	/*
	for (unsigned i = 0; i < terminal_states.size(); ++i)
	{
		if(ComparePosition(state,terminal_states[i]))
		{
			return true;
		}
	}
	*/
	if(state.elements[StateIdx::Position_DeliveryPoint] == -1 &&
		state.elements[StateIdx::Position_Packet] == -1
	)
	{
		return true;
	}
	return false;
}

bool DeliveryDrone1D::Check_Blocked_State(const SmartVector& state) const
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

bool DeliveryDrone1D::Check_Refuel_State(const SmartVector& state) const
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

bool DeliveryDrone1D::Check_Above_Packet(const SmartVector& state) const //whether you are on the packet or not
{
	SmartVector packetPos(1);
	packetPos.elements[0] = state.elements[StateIdx::Position_Packet];

	if(ComparePosition(state,packetPos))
	{
		return true;
	}

	return false;
}

bool DeliveryDrone1D::Check_Packet_Picked(const SmartVector& state) const
{
	SmartVector packetPos(1);
	packetPos.elements[0] = state.elements[StateIdx::Position_Packet];

	if(packetPos.elements[0] < 0)
	{
		return true;
	}

	return false;
}


bool DeliveryDrone1D::Check_Above_DeliveryPoint(const SmartVector& state) const //whether you are on the deliverypoint or not
{
	SmartVector deliveryPos(1);
	deliveryPos.elements[0] = state.elements[StateIdx::Position_DeliveryPoint];

	if(ComparePosition(state,deliveryPos))
	{
		return true;
	}

	return false;
}

bool DeliveryDrone1D::Check_Packet_Delivered(const SmartVector& state) const
{
	SmartVector packetPos(1);
	packetPos.elements[0] = state.elements[StateIdx::Position_Packet];

	SmartVector deliveryPos(1);
	deliveryPos.elements[0] = state.elements[StateIdx::Position_DeliveryPoint];

	if(packetPos.elements[0] < 0 &&
			deliveryPos.elements[0] < 0 )
	{
		return true;
	}

	return false;
}

void DeliveryDrone1D::Display_State(const SmartVector& agent)  const
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

	// PacketToBePicked 2609 - Sun - DotInTheMiddleOfCircle
	// PacketDeliveryStation 26F6 - Square four corners - Not Working
	// 25EC - White Up-Pointing Triangle with Dot

	// Future Use For MultiAgent:
	// Numbered items: 2781 and 278B

	// Emojis: https://apps.timwhitlock.info/emoji/tables/unicode
	// 1F4E6 - Package
	// 1F381 - Wrapped Present


	SmartVector packetPos(1);
	packetPos.elements[0] = agent.elements[StateIdx::Position_Packet];

	SmartVector deliveryPos(1);
	deliveryPos.elements[0] = agent.elements[StateIdx::Position_DeliveryPoint];

	cout<<"Displaying Current Status: [";
	for (int i = 0; i < StateIdx::TotalNumberOfStates; ++i) {
		cout<< " " << agent.elements[i];
	}
	cout<<" ] #"<<agent.index<<endl;

	//for (int pos = number_of_rows-1; pos >= 0; pos--)
	for (int pos = 0; pos < number_of_positions; pos++)
	{
		SmartVector cursorPos(1);
		cursorPos.elements[0] = pos;

		// Blocked State
		for (unsigned i = 0; i < blocked_states.size(); ++i)
		{
			if(ComparePosition(cursorPos, blocked_states[i]))
			{
				cout<<"\u2612";
				goto position_placed;
			}
		}

		// Terminal State
		for (unsigned i = 0; i < terminal_states.size(); ++i)
		{
			if(ComparePosition(cursorPos, terminal_states[i]))
			{
				if(rewards_of_terminal_states[i]>0)
				{
					if(ComparePosition(agent, cursorPos))
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
					if(ComparePosition(agent, cursorPos))
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

		// Refuel State
		for (unsigned i = 0; i < refuel_states.size(); ++i)
		{
			if(ComparePosition(cursorPos, refuel_states[i]))
			{
				if(ComparePosition(agent, cursorPos))
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

		// Packet
		if(ComparePosition(cursorPos, packetPos))
		{
			if(ComparePosition(agent, cursorPos))
			{
				cout<<"\033[32m\u2609\033[0m"; // Green Sun
				goto position_placed;
			}
			else
			{
				cout<<"\u2609"; // Sun
				goto position_placed;
			}
		}

		// Delivery Point
		if(ComparePosition(cursorPos, deliveryPos))
		{
			if(ComparePosition(agent, cursorPos))
			{
				cout<<"\033[32m\u25EC\033[0m"; // Green Delivery Point
				goto position_placed;
			}
			else
			{
				cout<<"\u25EC"; // Delivery Point
				goto position_placed;
			}
		}

		// Ordinary State
		if(ComparePosition(agent, cursorPos))
		{
			cout<<"\033[32m\u25A0\033[0m"; // Ordinary State
			goto position_placed;
		}
		else
		{
			cout<<"\u25A1"; // Ordinary State filled
			goto position_placed;
		}

position_placed:
		cout << " ";
		
	}
	cout << " " << endl;
	cout<<"Current Fuel: "<<agent.elements[StateIdx::Fuel]<<endl;
}

void DeliveryDrone1D::Display_Action(const SmartVector& action) const
{
	action.Print();
}

void DeliveryDrone1D::Display_Policy(const Representation& rep)  const
{
	cout<<"Display_Policy not implemented yet..."<<endl;
	abort();
	return;

	/*
	cout<<endl<<endl<<"Displaying Policy Status:"<<endl;

	int Policy[number_of_positions][fuel_max];
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

	for (int pos = 0; pos < number_of_positions; pos++)
	{
		for (int c = 0; c < number_of_columns; c++)
		{
			char ch = (Policy[pos][c]==4) ? 'o' : (Policy[pos][c]==0) ? '^' : (Policy[pos][c]==1) ? '>' : (Policy[pos][c]==2) ? '_' : '<';

			SmartVector state(2);
			state.elements[0] = pos;
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
	*/
}

//TODO Use Hashmaps instead of bruteforce search
int DeliveryDrone1D::Get_State_Index(const SmartVector& state) const
{
	/*
	vector<SmartVector> states = DeliveryDrone::Get_All_Possible_States();

	for (unsigned i = 0; i < states.size(); ++i) {
		if(state == states[i])
			return i;
	}
	*/
	if(representation_model != "TabularStateActionPair")
		return -1;

	std::unordered_map<std::string,int>::iterator val = state_indexes.find(state.Serialize());

	if(val!=state_indexes.end()) return val->second;

	cout<<endl<<endl<<"INVALID STATE!"<<endl<<endl;
	state.Print();

	cout<<"Controlled Abort..."<<endl;
	abort();
	return -1;
}

void DeliveryDrone1D::Test(void)
{
	cout<<"THIS IS TEST FUNCTION CALL:"<<endl;

	SmartVector state = Get_All_Possible_States()[10];
	SmartVector action = Get_Action_List(state)[0];

	for (int var = 0; var < 100; ++var) {
		SmartVector nextState = Get_Next_State(state,action);
		nextState.Print();
	}

}
