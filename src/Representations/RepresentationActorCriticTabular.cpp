/*
 * QValue.cpp
 *
 *  Created on: Mar 21, 2017
 *      Author: farabiahmed
 */

#include <Representations/RepresentationActorCriticTabular.hpp>

RepresentationActorCriticTabular::RepresentationActorCriticTabular(const Environment& env, const ConfigParser& cfg)
:Representation(env)
{
	Name = "RepresentationActorCriticTabular";

	// Get the total number of states
	// Total number of states are needed to form Q-Value (state-action pair) container.
	vector<SmartVector> states = env.Get_All_Possible_States();
	vector<SmartVector> actions;

	// Form Q-Value regarding to states and actions.
	// Then initialize it with zeros.

	// Create States First
	ActorValue = vector<vector<double>>(states.size());
	CriticValue = vector<double>(states.size());

	// Loop through each state, then create each action.
	for (unsigned i = 0; i < ActorValue.size(); ++i)
	{
		// Get available actions for each state
		actions = env.Get_Action_List(states[i]);
		ActorValue[i] = vector<double>(actions.size());
	}
}

RepresentationActorCriticTabular::~RepresentationActorCriticTabular() {
	// Delete Pointer
}

pair<int,double> RepresentationActorCriticTabular::Get_Greedy_Pair(const SmartVector& state) const
{
	assert(state.index	>=0 	&& state.index	< (int)CriticValue.size());

	// initialize max value
	double maxVal = CriticValue[state.index];

	vector<double>  actionValues = ActorValue[state.index];
	int maxArg = SoftMax(actionValues);

	return make_pair(maxArg,maxVal);
}


void RepresentationActorCriticTabular::Set_Value(const SmartVector& state, const SmartVector& action, double val)
{
	// Avoid the indexes that are out of limits.
	assert(state.index	>=0 	&& state.index	< (int)ActorValue.size());
	assert(action.index	>=0 	&& action.index	< (int)ActorValue[state.index].size());

	double alpha 	= 0.1;
	double beta		= 0.1;

	double delta = val - Get_Value(state,action);

	CriticValue[state.index] 				+= 	alpha 	* delta;
	ActorValue[state.index][action.index] 	+= 	beta 	* delta;
}

double RepresentationActorCriticTabular::Get_Value(const SmartVector& state, const SmartVector& action) const
{
	// Avoid the indexes that are out of limits.
	assert(state.index	>=0 	&& state.index	< (int)CriticValue.size());

	return CriticValue[state.index];
}

SmartVector RepresentationActorCriticTabular::Get_Policy(const SmartVector& state) const
{
	// Get Best Policy Index
	vector<double>  actionValues = ActorValue[state.index];

	int action_index = SoftMax(actionValues);

	return environment->Get_Action_List(state)[action_index];
}

int RepresentationActorCriticTabular::SoftMax(vector<double> &values) const
{
	vector<double> probabilities;

	// Calculate Softmax
	double sum_softmax = 0;
	for (unsigned i = 0; i < values.size(); ++i)
	{
		values[i] = exp(values[i]);
		sum_softmax += values[i];
	}

	for (unsigned i = 0; i < values.size(); ++i)
	{
		probabilities.push_back(values[i] / sum_softmax);
	}

	// Create an instance of experiment engine.
	ProbabilityDistribution *p = new DiscreteDistribution(probabilities);

	// Get an sample from engine.
	vector<int> outcomes = p->Run_Experiment(1);
	delete p;


	for (unsigned i = 0; i < outcomes.size(); ++i) {
		if(outcomes[i] == 1)
			return i;
	}

	return -1;
}

void RepresentationActorCriticTabular::Print_Value()
{
	cout<<endl<<"Displaying State-Action Pair Q Value:"<<endl;

	for (unsigned i = 0; i < CriticValue.size(); ++i) {

		cout<<"State #"<<i<< ":";

		cout <<  setw(10) << setprecision(5) << CriticValue[i]<< "   ";

		cout<<endl;
	}
}
