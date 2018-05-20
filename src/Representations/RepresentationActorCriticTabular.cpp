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

	ActionSelectorNumberOfSampling = cfg.GetValueOfKey< int >("ACTION_SELECTOR_SAMPLES",100);
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
	vector<double>	probabilities =  SoftMax(actionValues);

	int maxArg = ActionSelect(probabilities);

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

	vector<double>	probabilities =  SoftMax(actionValues);
	int action_index = ActionSelect(probabilities);
	return environment->Get_Action_List(state)[action_index];
}

vector<double> RepresentationActorCriticTabular::SoftMax(vector<double> &values) const
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

	return probabilities;
}

int RepresentationActorCriticTabular::ActionSelect(vector<double> probabilities) const
{
	if(ActionSelectorNumberOfSampling)
	{
		// Select Action By Sampling

		// Create an instance of experiment engine.
		ProbabilityDistribution *p = new DiscreteDistribution(probabilities);

		// Get an sample from engine.
		vector<int> outcomes = p->Run_Experiment(ActionSelectorNumberOfSampling);
		delete p;

		int maxVal=0;
		int maxArg=0;

		for (unsigned i = 0; i < outcomes.size(); ++i) {
			if(outcomes[i] > maxVal)
			{
				maxVal = outcomes[i];
				maxArg = i;
			}
		}

		return maxArg;
	}
	else
	{
		// Select Action By ArgMax

		// initialize max value
		double maxVal = probabilities[0];
		int maxArg=0;

		for (unsigned var = 0; var < probabilities.size(); ++var) {
			if(probabilities[var] > maxVal)
			{
				maxVal = probabilities[var];
				maxArg = var;
			}
		}

		return maxArg;
	}
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
