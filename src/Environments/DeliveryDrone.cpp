/*
 * DeliveryDrone.cpp
 *
 *  Created on: Sep, 22, 2019
 *      Author: farabiahmed
 */

#include <Environments/DeliveryDrone.hpp>

DeliveryDrone::DeliveryDrone(const ConfigParser& cfg) {

	//cout<<"Class DeliveryDrone Constructor"<<endl;

	Name = "Grid World Refuel Environment";

	//terminal_states = cfg.GetValueOfKey< vector<SmartVector> >("TERMINAL_STATES");

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

DeliveryDrone::~DeliveryDrone() {

	//cout<<"Class DeliveryDrone Destructor"<<endl;
}

double DeliveryDrone::Get_Reward(const SmartVector& currentState, const SmartVector& action, const SmartVector& nextState)
{
	double reward = 0.0;

	if(Check_Packet_Picked(nextState) && !Check_Packet_Picked(currentState))
	{
		reward += 0.2;
	}

	if(Check_Terminal_State(nextState))
	{
		reward += 1;
	}

	if(nextState.elements[StateIdx::Fuel] == 0) reward += -1;

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
	return reward+cost_action;
}

SmartVector DeliveryDrone::BurnFuel(const SmartVector &state)
{
	SmartVector temp = state;

	temp.elements[StateIdx::Fuel]--;

	if(Check_Refuel_State(temp))
	{
		temp.elements[StateIdx::Fuel] = fuel_max;
	}

	return temp;
}

vector<pair<SmartVector,double>> DeliveryDrone::Get_Transition_Probability(const SmartVector& currentState, const SmartVector& action)
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
	vector<SmartVector> actions = DeliveryDrone::Get_Action_List(currentState);

	// Get all states
	//vector<SmartVector> states = DeliveryDrone::Get_All_Possible_States();

	// Create Instance of return variable
	vector<pair<SmartVector,double>> state_probability;

	// Temporary Pair that holds next possible state and its probability.
	pair<SmartVector,double> tempPair;

	// State Candidate
	SmartVector stateCandid(StateIdx::TotalNumberOfStates);

	// North
	if(action.elements[0]==0)
	{

		if ( Get_Feasibility_Of_Action(currentState,actions[0], stateCandid) )
		{
			tempPair = make_pair(BurnFuel(stateCandid),pDes);
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
		Get_Feasibility_Of_Action(currentState,actions[1], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),pRight)) :
				state_probability.push_back(make_pair(currentState,pRight));

		Get_Feasibility_Of_Action(currentState,actions[3], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),pLeft)) :
				state_probability.push_back(make_pair(currentState,pLeft));

	}
	// East
	else if (action.elements[0]==1)
	{
		Get_Feasibility_Of_Action(currentState,actions[1], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),pDes)) :
				state_probability.push_back(make_pair(currentState,pDes));

		Get_Feasibility_Of_Action(currentState,actions[2], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),pRight)) :
				state_probability.push_back(make_pair(currentState,pRight));

		Get_Feasibility_Of_Action(currentState,actions[0], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),pLeft)) :
				state_probability.push_back(make_pair(currentState,pLeft));
	}
	// South
	else if (action.elements[0]==2)
	{
		Get_Feasibility_Of_Action(currentState,actions[2], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),pDes)) :
				state_probability.push_back(make_pair(currentState,pDes));

		Get_Feasibility_Of_Action(currentState,actions[3], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),pRight)) :
				state_probability.push_back(make_pair(currentState,pRight));

		Get_Feasibility_Of_Action(currentState,actions[1], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),pLeft)) :
				state_probability.push_back(make_pair(currentState,pLeft));
	}
	// West
	else if (action.elements[0]==3)
	{
		Get_Feasibility_Of_Action(currentState,actions[3], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),pDes)) :
				state_probability.push_back(make_pair(currentState,pDes));

		Get_Feasibility_Of_Action(currentState,actions[0], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),pRight)) :
				state_probability.push_back(make_pair(currentState,pRight));

		Get_Feasibility_Of_Action(currentState,actions[2], stateCandid) ?
				state_probability.push_back(make_pair(BurnFuel(stateCandid),pLeft)) :
				state_probability.push_back(make_pair(currentState,pLeft));
	}
	// Do Nothing
	else if (action.elements[0]==4)
	{
		state_probability.push_back(make_pair(currentState,1.0));
	}

	// Get the index of returned state
	for (unsigned i = 0; i < state_probability.size(); ++i) {

		// If the new state is on packet or deliverystation, update the state accordingly.
		if(Check_Packet_Picked(state_probability[i].first) && Check_Above_DeliveryPoint(state_probability[i].first))
		{
			state_probability[i].first.elements[StateIdx::Row_DeliveryPoint] = -1;
			state_probability[i].first.elements[StateIdx::Col_DeliveryPoint] = -1;
		}
		else if(Check_Above_Packet(state_probability[i].first))
		{
			state_probability[i].first.elements[StateIdx::Row_Packet] = -1;
			state_probability[i].first.elements[StateIdx::Col_Packet] = -1;
		}

		state_probability[i].first.index = DeliveryDrone::Get_State_Index(state_probability[i].first);
	}

	return state_probability;
}

bool DeliveryDrone::Get_Feasibility_Of_Action(const SmartVector& state, const SmartVector& action, SmartVector& statecandidate) const
{
	statecandidate = state;

	int r = state.elements[StateIdx::Row_Agent];
	int c = state.elements[StateIdx::Col_Agent];
	int f = state.elements[StateIdx::Fuel]; //fuel

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

		statecandidate.elements[StateIdx::Row_Agent]--;
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

		statecandidate.elements[StateIdx::Col_Agent]++;
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

		statecandidate.elements[StateIdx::Row_Agent]++;
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

		statecandidate.elements[StateIdx::Col_Agent]--;
	}

	return true;
}

vector<SmartVector> DeliveryDrone::Get_All_Possible_States() const
{
	static vector<SmartVector> states;

	if(states.size()==0)
	{
		int index=0;

		for (int r = 0; r < number_of_rows; r++) {
			for (int c = 0; c < number_of_columns; c++) {
				for (int f = fuel_max; f >= 0; f--) {
					for (int r_packet = -1; r_packet < number_of_rows; r_packet++) { //-1 means packet picked
						for (int c_packet = -1; c_packet < number_of_columns; c_packet++) {
							for (int r_deliverypoint = -1; r_deliverypoint < number_of_rows; r_deliverypoint++) { // -1 means packet delivered
								for (int c_deliverypoint = -1; c_deliverypoint < number_of_columns; c_deliverypoint++) {

									SmartVector state(StateIdx::TotalNumberOfStates);

									state.elements[StateIdx::Row_Agent] = r;
									state.elements[StateIdx::Col_Agent] = c;

									state.elements[StateIdx::Fuel] = f;

									state.elements[StateIdx::Row_Packet] = r_packet;
									state.elements[StateIdx::Col_Packet] = c_packet;

									state.elements[StateIdx::Row_DeliveryPoint] = r_deliverypoint;
									state.elements[StateIdx::Col_DeliveryPoint] = c_deliverypoint;

									// Check Packet Position is valid
									if( (r_packet == -1 && c_packet>-1) || (c_packet == -1 && r_packet>-1))
										continue;

									// Check Delivery Position is valid
									if( (r_deliverypoint == -1 && c_deliverypoint>-1) || (c_deliverypoint == -1 && r_deliverypoint>-1))
										continue;

									// If there is packet but no delivery station, skip it.
									if(r_packet>=0 && (r_deliverypoint==-1))
										continue;

									// Check Packet and Delivery Point overlapped
									if(r_packet == r_deliverypoint && c_packet == c_deliverypoint && r_packet>-1)
										continue;

									// Check whether the position on the blocked state
									bool invalid = false;
									for (size_t i = 0; i < blocked_states.size(); ++i)
									{
										if( r == blocked_states[i].elements[0] && c == blocked_states[i].elements[1])
										{
											invalid = true;
											break;
										}

										if( r_packet == blocked_states[i].elements[0] && c_packet == blocked_states[i].elements[1])
										{
											invalid = true;
											break;
										}

										if( r_deliverypoint == blocked_states[i].elements[0] && c_deliverypoint == blocked_states[i].elements[1])
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
				}
			}
		}
	}
	return states;
}

vector<SmartVector> DeliveryDrone::Get_Action_List(const SmartVector& state) const
{
	static vector<SmartVector> actions;

	if(actions.size()==0)
	{
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
	}

	return actions;
}


SmartVector DeliveryDrone::Get_Next_State(const SmartVector& state, const SmartVector& action){

	SmartVector vec;

	vector<pair<SmartVector,double>> nextStateCandidates = DeliveryDrone::Get_Transition_Probability(state,action);

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


SmartVector DeliveryDrone::Get_Initial_State()
{
	SmartVector vec;

	vec = this->Get_All_Possible_States()[0];

	return vec;
}

SmartVector DeliveryDrone::Get_Random_State()
{
	SmartVector state;
	state = Environment::Get_Random_State();

	while(state.elements[StateIdx::Fuel] != fuel_max)
	{
		state = Environment::Get_Random_State();
	}

	return state;
}

bool DeliveryDrone::ComparePosition(const SmartVector& state1, const SmartVector& state2) const
{
	if( (state1.elements[0]==state2.elements[0]) && (state1.elements[1]==state2.elements[1]) )
	{
		return true;
	}

	return false;
}

bool DeliveryDrone::Check_Terminal_State(const SmartVector& state) const
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
	if(state.elements[StateIdx::Row_DeliveryPoint] == -1 &&
		state.elements[StateIdx::Col_DeliveryPoint] == -1 &&
		state.elements[StateIdx::Row_Packet] == -1 &&
		state.elements[StateIdx::Col_Packet] == -1)
		{
			return true;
		}
	return false;
}

bool DeliveryDrone::Check_Blocked_State(const SmartVector& state) const
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

bool DeliveryDrone::Check_Refuel_State(const SmartVector& state) const
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

bool DeliveryDrone::Check_Above_Packet(const SmartVector& state) const //whether you are on the packet or not
{
	SmartVector packetPos(2);
	packetPos.elements[0] = state.elements[StateIdx::Row_Packet];
	packetPos.elements[1] = state.elements[StateIdx::Col_Packet];

	if(ComparePosition(state,packetPos))
	{
		return true;
	}

	return false;
}

bool DeliveryDrone::Check_Packet_Picked(const SmartVector& state) const
{
	SmartVector packetPos(2);
	packetPos.elements[0] = state.elements[StateIdx::Row_Packet];
	packetPos.elements[1] = state.elements[StateIdx::Col_Packet];

	if(packetPos.elements[0] < 0 && packetPos.elements[1] < 0)
	{
		return true;
	}

	return false;
}


bool DeliveryDrone::Check_Above_DeliveryPoint(const SmartVector& state) const //whether you are on the deliverypoint or not
{
	SmartVector deliveryPos(2);
	deliveryPos.elements[0] = state.elements[StateIdx::Row_DeliveryPoint];
	deliveryPos.elements[1] = state.elements[StateIdx::Col_DeliveryPoint];

	if(ComparePosition(state,deliveryPos))
	{
		return true;
	}

	return false;
}

bool DeliveryDrone::Check_Packet_Delivered(const SmartVector& state) const
{
	SmartVector packetPos(2);
	packetPos.elements[0] = state.elements[StateIdx::Row_Packet];
	packetPos.elements[1] = state.elements[StateIdx::Col_Packet];

	SmartVector deliveryPos(2);
	deliveryPos.elements[0] = state.elements[StateIdx::Row_DeliveryPoint];
	deliveryPos.elements[1] = state.elements[StateIdx::Col_DeliveryPoint];

	if(packetPos.elements[0] < 0 && packetPos.elements[1] < 0 &&
			deliveryPos.elements[0] < 0 && deliveryPos.elements[1] < 0)
	{
		return true;
	}

	return false;
}

void DeliveryDrone::Display_State(const SmartVector& agent)  const
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


	SmartVector packetPos(2);
	packetPos.elements[0] = agent.elements[StateIdx::Row_Packet];
	packetPos.elements[1] = agent.elements[StateIdx::Col_Packet];

	SmartVector deliveryPos(2);
	deliveryPos.elements[0] = agent.elements[StateIdx::Row_DeliveryPoint];
	deliveryPos.elements[1] = agent.elements[StateIdx::Col_DeliveryPoint];

	cout<<"Displaying Current Status: [";
	for (int i = 0; i < StateIdx::TotalNumberOfStates; ++i) {
		cout<< " " << agent.elements[i];
	}
	cout<<" ] #"<<agent.index<<endl;

	//for (int r = number_of_rows-1; r >= 0; r--)
	for (int r = 0; r < number_of_rows; r++)
	{
		for (int c = 0; c < number_of_columns; c++)
		{
			SmartVector cursorPos(2);
			cursorPos.elements[0] = r;
			cursorPos.elements[1] = c;

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
	}

	cout<<"Current Fuel: "<<agent.elements[2]<<endl;
}

void DeliveryDrone::Display_Action(const SmartVector& action) const
{
	action.Print();
}

void DeliveryDrone::Display_Policy(const Representation& rep)  const
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
int DeliveryDrone::Get_State_Index(const SmartVector& state) const
{
	/*
	vector<SmartVector> states = DeliveryDrone::Get_All_Possible_States();

	for (unsigned i = 0; i < states.size(); ++i) {
		if(state == states[i])
			return i;
	}
	*/

	std::unordered_map<std::string,int>::iterator val = state_indexes.find(state.Serialize());

	if(val!=state_indexes.end()) return val->second;

	cout<<endl<<endl<<"INVALID STATE!"<<endl<<endl;
	state.Print();

	cout<<"Controlled Abort..."<<endl;
	abort();
	return -1;
}

void DeliveryDrone::Test(void)
{
	cout<<"THIS IS TEST FUNCTION CALL:"<<endl;

	SmartVector state = Get_All_Possible_States()[10];
	SmartVector action = Get_Action_List(state)[0];

	for (int var = 0; var < 100; ++var) {
		SmartVector nextState = Get_Next_State(state,action);
		nextState.Print();
	}

}
