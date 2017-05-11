/*
 * Blocksworld.cpp
 *
 *  Created on: Apr 26, 2017
 *      Author: farabiahmed
 */

#include <Environments/Blocksworld.hpp>
#include "Representations/Representation.hpp" // Due to the forward declaration we must include this headers in cpp file to avoid recursive inclusion.

Blocksworld::Blocksworld(const ConfigParser& cfg)
{
	Name = "BlocksWorld Environment";

	number_of_blocks = cfg.GetValueOfKey<int>("NUMBER_OF_BLOCKS",2);

	reward_goal = cfg.GetValueOfKey<double>("REWARD_GOAL",1.0);

	cost_action = cfg.GetValueOfKey<double>("COST_ACTION",-0.001);

	probability_of_random_action = cfg.GetValueOfKey<double>("PROBABILITY_OF_RANDOM_ACTION",0.20);
	
}

Blocksworld::~Blocksworld() {
	// TODO Auto-generated destructor stub
}

double Blocksworld::Get_Reward(const SmartVector& currentState, const SmartVector& action, const SmartVector& nextState)
{
	if(action.elements[0]!=0)
	{
		if(nextState == Get_Goal_State())
			return reward_goal;
		else
			return cost_action;
	}
	return 0;
}

vector<pair<SmartVector,double>> Blocksworld::Get_Transition_Probability(const SmartVector& currentState, const SmartVector& action)
{
	double pDes = 1.0 - probability_of_random_action; 	// Dropping proper slot
	double pLeft = probability_of_random_action*0.5; 	// Dropping left slot
	double pRight = probability_of_random_action*0.5; 	// Dropping right slot

	int block_id;
	int slot_id;

	// Get the given block and slot
	block_id = action.elements[0];
	slot_id = action.elements[1];

	vector<pair<SmartVector,double>> state_probability;
	SmartVector nextState = currentState;

	// Move the block to top of the given slot.
	// (blocki,s)

	// Get the current slot status
	vector<vector <int>> slots = Get_Slots(currentState);

	if( action.elements[0] > 0 ) // If the action different from "do-nothing"
	{
		// Proper Action
		if(slots[slot_id].size())
		{
			// Get the id of block residing on the top of the given slot.
			int top_block_id = slots[slot_id].back();

			// Update the state
			nextState.elements[block_id-1] = top_block_id;
		}
		else
			// Update the state with ground.
			nextState.elements[block_id-1] = 0;

		nextState.index = Get_State_Index(nextState);
		state_probability.push_back(make_pair(nextState,pDes));

		if(pLeft>0)
		{
			// If we drop it to the right of the proper slot:
			slot_id = action.elements[1]+1 ;
			if(slot_id >= (int) number_of_blocks) slot_id=0;	// We assume slots are circular

			if(slots[slot_id].size())
			{
				// Get the id of block residing on the top of the given slot.
				int top_block_id = slots[slot_id].back();

				if(top_block_id!=block_id)
					// Update the state
					nextState.elements[block_id-1] = top_block_id;
				else
					nextState = currentState;
			}
			else
				// Update the state with ground.
				nextState.elements[block_id-1] = 0;

			nextState.index = Get_State_Index(nextState);
			state_probability.push_back(make_pair(nextState,pRight));
		}

		if(pRight>0)
		{
			// If we drop it to the left of the proper slot:
			slot_id = action.elements[1]-1 ;
			if(slot_id<0) slot_id=number_of_blocks-1; 	// We assume slots are circular

			if(slots[slot_id].size())
			{
				// Get the id of block residing on the top of the given slot.
				int top_block_id = slots[slot_id].back();

				if(top_block_id!=block_id)
					// Update the state
					nextState.elements[block_id-1] = top_block_id;
				else
					nextState = currentState;
			}
			else
				// Update the state with ground.
				nextState.elements[block_id-1] = 0;

			nextState.index = Get_State_Index(nextState);
			state_probability.push_back(make_pair(nextState,pLeft));
		}
	}
	return state_probability;
}

vector<SmartVector> Blocksworld::Get_All_Possible_States() const
{
	// This algorithm works like "base aritmetic"
	// -Initial state for n=3 is [0 0 0]
	// -If we increase it sequentially we get [1 0 0], [2 0 0], [3 0 0], [0 1 0] and so on.

	vector<SmartVector> states;		// Vector of all possible states that will return at the end of the function.

	unsigned index_of_space = 0; 	// Holds the sequence-number of the state in the set of all possible states.

	unsigned position_of_block;  	// This variable works as memory of value of the current block position.
									// we will increase that variable and then check it if it is in the limits.

	unsigned index_of_block=0;		// Holds the blockid. By the help of that varible,
									// we decide that which blockid should be increased.

	// Initialize a state, we will modify that state in every loop,
	// if it will become a feasible state, then we will load it to
	// vector of states to return.
	SmartVector state(number_of_blocks);
	state.Initialize(0);

	// Put first element of the space.
	state.index = index_of_space++;
	states.push_back(state);

	// Start the loop to find all possible states.
	while(index_of_block<number_of_blocks)
	{
		// Get the position of the block that we are interested in,
		// then increase it to reach a new possible state.
		position_of_block = state.elements[index_of_block];
		position_of_block++;

		// If the new position of the block is not inside the limits,
		// reset the current position and advance to next block,
		// then skip that state.
		if(position_of_block > number_of_blocks)
		{
			state.elements[index_of_block] = 0;
			index_of_block++;
		}

		// If the new position of the block is inside the limits,
		// check it if it is feasible.
		// If the new state is feasible, get it.
		else
		{
			// Produce new state from current one.
			state.elements[index_of_block] = position_of_block;

			// If the state just produced is valid, get it.
			if(!Check_Blocked_State(state))
			{
				state.index = index_of_space++;
				states.push_back(state);
			}

			// Clear the flag (return to first block)
			index_of_block = 0;
		}
	}

	return states;
}

vector<SmartVector> Blocksworld::Get_Action_List(const SmartVector& state) const
{
	// Vector of action is 2 by 1 vector.
	// For a given state, there are three different cases;
	// Case #1: If it is already a goal state, "do-nothing (0,N/A)".
	// Case #2: MOVE (i,s) where i>0, s={0,1,...,n-1}
	//		- Step1: Get the block from top of the slot in which it is located.
	//		- Step2: Release it to the top of the another given slot (s)
	// 		- Note: Since our state doesn't hold slot number for blocks, it doesn't make a sense
	//		to remove a single block from one slot to another empty slot. Yet, we include it,
	// 		to populate action options.

	vector<vector<int>> slots = Get_Slots(state);

	// Create instance of return value
	vector<SmartVector> actions;

	// index of vector in the set
	int index_of_set=0;

	// Do-Nothing Action Always Available.
	SmartVector action(2);
	action.Initialize(0);
	action.index = index_of_set++;
	actions.push_back(action);

	// Case #1:
	// If we are in Goal state, no need to take any action, so just add "do-nothing".
	if(state==Get_Goal_State())
	{
		return actions;
	}
	else
	{
		for (unsigned i = 0; i < slots.size(); ++i) {

			// Add Action Move (i,s) where i>0, s={0,1,...,n-1}
			// Loop through each slot, and search for slots
			if(slots[i].size() > 0)
			{
				action = SmartVector(2);
				action.elements[0] = slots[i].back();

				for (unsigned j = 0; j < slots.size(); ++j) {
					// Skip the same slot
					if(i!=j)
					{
						// Add to list
						action.elements[1] = j;
						action.index = index_of_set++;
						actions.push_back(action);

						// If the last slot was an empty slot, stop the loop after adding action.
						//if(slots[j].size()==0)
						//	break;
					}
				}
			}
		}
		/*
		// Do the Unstack Actions First
		for (unsigned i = 0; i < slots.size(); ++i) {

			// Add Action UNSTACK (j,N/A) where j<0
			// Loop through each slot, and search for slots that have more than one block.
			if(slots[i].size() > 1)
			{
				action = SmartVector(2);
				action.elements[0] = -1 * slots[i].back();

				for (unsigned j = 0; j < slots.size(); ++j) {
					// Skip the same slot
					if(i!=j)
					{
						// Add to list
						action.elements[1] = j;
						action.index = index_of_set++;
						actions.push_back(action);

						// If the last slot was an empty slot, stop the loop after adding action.
						//if(slots[j].size()==0)
						//	break;
					}
				}

			}
		}

		// Do the Stack Actions
		for (unsigned i = 0; i < slots.size(); ++i) {

			// Check if stack full
			bool is_slot_full = (slots[i].size() == number_of_blocks);
			bool is_slot_empty = (slots[i].size() == 0);

			// Add Action STACK (i,s) where i>0, s={0,1,...,n-1}
			// If slot is full, stacking won't be meaningful, Go blank for it.
			// If slot has some blocks less than total number of blocks,
			// you can then stack them to another slots. Let's find them.
			if(!is_slot_full && !is_slot_empty)
			{
				// Get the top block.
				action = SmartVector(2);
				action.elements[0] = slots[i].back();

				// Put it to different slot.
				// Add each different slot to list until first empty slot.
				// Since stacking to different empty slots is not meaningful,
				// because their state value will be same as 0 (ground).
				for (unsigned j = 0; j < slots.size(); ++j) {

					// Skip the same slot
					if(i!=j)
					{
						// Add
						action.elements[1] = j;
						action.index = index_of_set++;
						actions.push_back(action);

						// If the last slot was an empty slot, stop the loop after adding action.
						//if(slots[j].size()==0)
						//	break;
					}
				}
			}
		}
		*/
	}

	return actions;
}

SmartVector Blocksworld::Get_Initial_State()
{
	SmartVector vec(number_of_blocks);

	vec.Initialize(0);
	vec.index = 0;

	return vec;
}

SmartVector Blocksworld::Get_Goal_State(void) const
{
	SmartVector vec(number_of_blocks);
	vec.Initialize(0);

	for (unsigned i = 0; i < number_of_blocks; ++i) {
		vec.elements[i] = i;
	}

	return vec;
}


bool Blocksworld::Check_Terminal_State(const SmartVector& state)
{
	// First of all just check if the first block is on the ground.
	// If the first block is on the ground then check the others if thay are arranged in order.
	// Example: state=[0 1 2 3 ... n-1]' is terminal state where n is the total number of blocks.

	if(state.elements[0]!=0)
		return false;

	int index=1;
	while(index < state.dimension)
	{
		int diff = state.elements[index] - state.elements[index-1];

		if( diff != 1)
			return false;

		index++;
	}
	return true;
}

bool Blocksworld::Check_Blocked_State(const SmartVector& state) const
{
	// A state is not-feasible or blocked if:
	// 		- Case 1: A block is over a non-existing block.
	// 		- Case 2: A block is over itself.
	//		- Case 3: More than one block is over same block.
	//		- Case 4: None Of the blocks on the ground.
	//		- Case 5: Recursive order occurred. (Most tricky one is checked after all.)
	// Since checking case 5 is not as easy as it seems, it causes us to use
	// different for loops to check each of them. It is advised to improve that function
	// later.


	int block_id, position, bottom_block_id, bottom_block_position;
	int number_of_blocks = state.dimension;

	int number_of_grounded_blocks=0;


	for (int i = 0; i < number_of_blocks; ++i) {

		// Get the block id from the index of the state.
		// Remember: state = [block1, block2, ... blockn] where n is the total number of blocks.
		// blockid can be an element of {1,2,3,...n}
		block_id = i+1;

		// Get the position of the blockj
		// Position means which block is under me?
		// position can be an element of {0,1,2,3,...n}
		position = state.elements[i];

		// Case 1:
		// -A block is over a non-existing block if the position of that block is bigger
		// than the total number of blocks.
		if(position > number_of_blocks)
			return true;

		// Case 2:
		// -A block is over itself if the position of that is equal to itself.
		if(block_id == position)
			return true;

		// Case 3:
		// -More than one block is over same block.
		// -This case is valid if the block is not on the ground.
		// -Brute Force Way: Loop through all other elements in the state and
		// search for same position
		if(position>0)
		{
			for (int j = i; j < number_of_blocks; ++j) {
				if(i!=j)
				{
					if(state.elements[j] == position)
						return true;
				}
			}
		}

		// Case 4:
		// Keep the number of total grounded blocks.
		// At the end of the function check it.
		if(position==0) number_of_grounded_blocks++;
	}

	// Case 4: Continue...
	if(number_of_grounded_blocks==0) return true;


	// Case 5:
	// -If a block is on the another block, the bottom block can not be on the top block.
	// -This case is valid if the block is not on the ground.
	// -It can be occurred if more than 2 blocks is ordered recursively.
	for (int i = 0; i < number_of_blocks; ++i) {
		block_id = i+1;
		position = state.elements[i];

		if(position>0)
		{
			bottom_block_id = position;
			bottom_block_position = state.elements[bottom_block_id-1];

			while( (bottom_block_position) != 0 )
			{
				if(bottom_block_position == block_id) // Circular order without touching ground.
				{
					//vec.Print();
					return true;
				}
				else if(bottom_block_position == 0)
					// Break the loop if one of the below blocks is on the ground.
					break;
				else
				{
					// Continue to Search if bottom block is on the ground.
					bottom_block_id = bottom_block_position;
					bottom_block_position = state.elements[bottom_block_id-1];
				}
			}

		}
	}
	return false;
}

int Blocksworld::Get_State_Index(const SmartVector& state) const
{
	vector<SmartVector> states = Get_All_Possible_States();

	for (unsigned i = 0; i < states.size(); ++i) {
		if(state == states[i])
			return i;
	}

	cout<<endl<<endl<<"INVALID STATE!"<<endl<<endl;
	return -1;
}

SmartVector Blocksworld::Get_Next_State(const SmartVector& state, const SmartVector& action)
{
	// Actions:
	// Case #1: "do-nothing (0,N/A)".
	// Case #2: MOVE (i,s) where i>0, s={0,1,...,n-1}


	SmartVector nextState = state;

	/*
	unsigned block_id, slot_id;

	if(action.elements[0]==0)
	{
		// Do nothing
	}
	else if(action.elements[0]<0)
	{
		// Unstack the block and place it to first empty slot.
		// (j,N/A)

		block_id = -action.elements[0];
		nextState.elements[block_id-1] = 0;
	}
	else if(action.elements[0]>0)
	{
		// Stack the block to top of the tower.
		// (i,s)

		// Get the given block and slot
		block_id = action.elements[0];
		slot_id = action.elements[1];

		// Get the current slot status
		vector<vector <int>> slots = Get_Slots(state);


		if(slots[slot_id].size())
		{
			// Get the id of block residing on the top of the given slot.
			int top_block_id = slots[slot_id].back();

			// Update the state
			nextState.elements[block_id-1] = top_block_id;
		}
		else
			// Update the state with ground.
			nextState.elements[block_id-1] = 0;
	}

	nextState.index = Get_State_Index(nextState);
	*/

	return nextState;
}

void Blocksworld::Display_State(const SmartVector& state) const
{
	/// Example State=[0 4 2 0]'
	/// Block 1 is on the ground.
	/// Block 2 is on the block 4.
	/// Block 3 is on the block 3.
	/// Block 4 is on the ground.
	///
	/// In order to display state graphically we need to convert that state to more intuitive format.
	/// Phase#1: First of all we will convert that "state" to "blocks_above" vector that will show which block is on the other.
	/// Phase#2: Then we will convert "blocks_above" vector to "slots" vector that will show that which slot holds which blocks.
	/// Phase#3: We can then easily draw graphically blocks state to check our results intuitively.
	/// Thus, we will have turned the state to something that we can easily understand.


	int width_of_field = log10(number_of_blocks) + 1; 	/// In order to increase the readability we will add some zeros before the
														/// the block id's. Instead of writing 1-15-119, "width_of_field" will help
														/// us write that as 001-015-119.

	vector<vector<int>> slots; 							/// That variable will store the block ids' that are hold in related slot.
																	///
																	/// For 4 slots and 4 blocks:
																	/// slots[0] = 1	 	(Block 1 is in the Slot#0)
																	/// slots[1] = 4,2,3 	(Blocks 4-2-3 are in the Slot#1)
																	/// slots[2] = -		(Slot#2 holds nothing.)
																	/// slots[3] = -		(Slot#3 holds nothing.)

	/// Phase #1 and Phase #2
	slots = Get_Slots(state);

	/// Phase #3
	/// Draw the slots to visualize state.
	cout << endl << "Displaying State: [";
	for (int i = 0; i < state.dimension; ++i) {
		cout << setfill(' ') << setw(width_of_field+2) << state.elements[i];
	}
	cout << setfill(' ') << setw(width_of_field+2) << "]' with index of: "<<state.index;

	if(Check_Blocked_State(state))
		cout<<endl<<"\033[31m"<<"This state is not valid."<<"\033[0m";

	//cout << "T" <<setfill('-') << setw(width_of_field) << ">" <<" [ ";

	for (unsigned i = 0; i < slots.size(); ++i)
	{
		cout << endl << "S" <<setfill('0') << setw(width_of_field) << i+1 << " [ ";

		for (unsigned j = 0; j < slots[i].size(); ++j) {
			cout<<"["<<  setfill('0') << setw(width_of_field) << slots[i][j]<<"] ";
		}
	}

}

void Blocksworld::Display_Action(const SmartVector& action) const
{
	unsigned block_id = abs(action.elements[0]);
	unsigned slot_id = abs(action.elements[1]);

	if(action.elements[0] == 0)
		cout << "Do Nothing" << endl;

	if(action.elements[0] > 0)
		cout << "Move Block #"<<  block_id << " to Slot #" << slot_id+1 << endl;
}

void Blocksworld::Display_Policy(const Representation& rep)  const
{
	/// Get The States
	vector<SmartVector> States = Get_All_Possible_States();

	for (unsigned i = 0; i < States.size(); ++i)
	{
		// Print Current State
		SmartVector state = States[i];
		Display_State(state);

		/// Get The Action List
		vector<SmartVector> Actions = Get_Action_List(state);

		/// Get max Q value for this state. <maxArg,maxVal>
		unsigned maxArg 	= rep.Get_Greedy_Pair(state).first;

		cout << endl << "Available Actions: " << endl;
		for (unsigned j = 0; j < Actions.size(); ++j) {

			// Make Colorful Line
			if(maxArg == j)
				cout<<"\033[32m";

			// Print index of action
			cout<<j<<". ";

			// Get current action
			SmartVector action = Actions[j];

			// Print details of action
			unsigned block_id = abs(action.elements[0]);
			unsigned slot_id = abs(action.elements[1]);

			if(action.elements[0] == 0)
				cout << "Do Nothing"<< "\t\t\t";

			if(action.elements[0] > 0)
				cout << "Move Block #"<<  block_id << " to Slot #" << slot_id+1 << "\t";

			cout << " Value: " << std::right <<setfill(' ') << setw(8) << setprecision(5) << rep.Get_Value(state,action) <<"\033[0m"<< endl;
		}
		cout << endl;
	}

	cout << endl;
}

vector<vector<int>> Blocksworld::Get_Slots( const SmartVector& state)  const
{
	/// This process is completed in two phases:
	/// Phase#1: First of all we will convert the "state" into "blocks_above" vector that will show which block is on the other.
	/// Phase#2: Then we will convert "blocks_above" vector to "slots" vector that will show that which slot holds which blocks.

	int number_of_blocks = state.size();		/// Number of blocks are equal to our state dimension. Since each elements of the state
	 	 	 	 	 	 	 	 	 	 	 	/// corresponds to position of related block.

	int number_of_slots = number_of_blocks;		/// Total slots that we hold our blocks are equal to number of blocks for now.
	 	 	 	 	 	 	 	 	 	 	 	/// Our blocks are standing on that slots before being placed to an appropriate
	 	 	 	 	 	 	 	 	 	 	 	/// location on the tower.

	vector<vector<int>> blocks_above(number_of_blocks + 1); 		/// That variable will store the block ids's that are on the other blocks.
																	/// ! +1 for ground level, a block can be on the ground or other blocks.
																	/// index of the vector shows ground(0) or other block id.
																	///
																	/// For 4 blocks
																	/// blocks_above[0] = 1,4    (Blocks 1-4 are on the ground.)
																	/// blocks_above[1] = -		(There are no blocks on the block#1)
																	/// blocks_above[2] = 3		(Block 3 is on the block#2)
																	/// blocks_above[3] = -		(There are no blocks on the block#3)
																	/// blocks_above[4] = 2		(Block 2 is on the block#4)

	vector<vector<int>> slots(number_of_slots); 					/// That variable will store the block ids' that are hold in related slot.
																	///
																	/// For 4 slots and 4 blocks:
																	/// slots[0] = 1	 	(Block 1 is in the Slot#0)
																	/// slots[1] = 4,2,3 	(Blocks 4-2-3 are in the Slot#1)
																	/// slots[2] = -		(Slot#2 holds nothing.)
																	/// slots[3] = -		(Slot#3 holds nothing.)


	/// Phase #1
	/// Lets Start with converting "state" to "blocks_above"
	/// Example: state=[ 0 , 4 , 2 , 0]'  ->   blocks_above = [ 1-4 , NA , 3 , NA , 2 ]'
	for (int block_id = 1; block_id  <= number_of_blocks; block_id++) {

		int block_below = state.elements[block_id-1];

		blocks_above[ block_below ].push_back(block_id);
	}

	/// Phase #2
	/// Let's convert "blocks_above" to "slots".
	/// Example: blocks_above = [ 1-4 , NA , 3 , NA , 2 ]' -> slots=[ 1 , 4-2-3, NA, NA]
	/// Since we will recursively check the whether there is more blocks on the ground blocks,
	/// it is enough to just check ground blocks only.
	/// It will automatically place other blocks to slots by the while loop.
	for (unsigned j = 0; j < blocks_above[0].size(); ++j) {

		int block_id = blocks_above[0][j];

		slots[j].push_back( block_id );

		// Check if there is a block over that block with this block_id.
		while(blocks_above[block_id].size()>0)
		{
			block_id = blocks_above[block_id][0];
			slots[j].push_back( block_id );
		}
	}

	return slots;
}

void Blocksworld::Test(void)
{
	cout<<endl<<"Blocksworld::Test Not Implemented Yet"<<endl;
}

