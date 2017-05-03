/*
 * QValue.cpp
 *
 *  Created on: Mar 21, 2017
 *      Author: farabiahmed
 */

#include <Representations/StateActionValue.hpp>

StateActionValue::StateActionValue(const Environment& env, const ConfigParser& cfg)
:Representation(env)
{
	// Get the total number of states
	// Total number of states are needed to form Q-Value (state-action pair) container.
	vector<SmartVector> states = env.Get_All_Possible_States();
	vector<SmartVector> actions;


	// Get the total number of actions
	// Total number of actions are needed to form Q-Value (state-action pair) container.

	// Form Q-Value regarding to states and actions.
	// Then initialize it with zeros.

	// Create States First
	Qvalue = vector<vector<double>>(states.size());

	// Loop through each state, then create each action.
	for (unsigned i = 0; i < Qvalue.size(); ++i)
	{
		// Get available actions for each state
		actions = env.Get_Action_List(states[i]);
		Qvalue[i] = vector<double>(actions.size());
	}

	/*
	// Create states
	Qvalue = new double* [numberOfStates];

	// Loop through each states to assign the required number of actions.
	for (int state = 0; state < numberOfStates; ++state)
	{
		// Assign actions to each state.
		Qvalue[state] = new double[numberOfActions];

		// Initialize them.
		for (int action = 0; action < numberOfActions; ++action)
			Qvalue[state][action] = 0;
	}
	*/
}

StateActionValue::~StateActionValue() {
	// Delete Pointer

	/*
	for (int row = 0; row < numberOfStates; ++row)
	  delete[] Qvalue[row];

	delete[] Qvalue;
	*/
}

pair<int,double> StateActionValue::Get_Greedy_Pair(SmartVector state)
{
	assert(state.index	>=0 	&& state.index	< (int)Qvalue.size());

	int size_of_actions = Qvalue[state.index].size();

	// initialize max value
	double maxVal = Qvalue[state.index][0];
	int maxArg=0;

	for (int var = 0; var < size_of_actions; ++var) {
		if(Qvalue[state.index][var] > maxVal)
		{
			maxVal = Qvalue[state.index][var];
			maxArg = var;
		}
	}

	return make_pair(maxArg,maxVal);
}


void StateActionValue::Set_Value(const SmartVector& state, const SmartVector& action, double val)
{
	// Avoid the indexes that are out of limits.
	assert(state.index	>=0 	&& state.index	< (int)Qvalue.size());
	assert(action.index	>=0 	&& action.index	< (int)Qvalue[state.index].size());

	Qvalue[state.index][action.index] = val;
}

list<double> StateActionValue::Get_Value(SmartVector state)
{
	list<double> vecList;

	cout<<endl<<"NOT IMPLEMENTED YET"<<endl<<endl;

	return vecList;
}

double StateActionValue::Get_Value(const SmartVector& state, const SmartVector& action)
{
	// Avoid the indexes that are out of limits.
	assert(state.index	>=0 	&& state.index	< (int)Qvalue.size());
	assert(action.index	>=0 	&& action.index	< (int)Qvalue[state.index].size());

	return Qvalue[state.index][action.index];
}

SmartVector StateActionValue::Get_Policy(SmartVector state)
{
	SmartVector vec;

	cout<<endl<<"NOT IMPLEMENTED YET"<<endl<<endl;

	return vec;
}

void StateActionValue::Print_Value()
{
	cout<<endl<<"Displaying State-Action Pair Q Value:"<<endl;

	for (unsigned i = 0; i < Qvalue.size(); ++i) {

		cout<<"State #"<<i<< ":";

		for (unsigned j = 0; j < Qvalue[i].size(); ++j) {
			cout <<  setw(10) << setprecision(5) << Qvalue[i][j]<< "   ";
		}
		cout<<endl;
	}
}
