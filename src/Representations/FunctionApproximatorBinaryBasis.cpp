/*
 * FunctionApproximatorBinaryBasis.cpp
 *
 *  Created on: May 8, 2017
 *      Author: farabiahmed
 */

#include <Representations/FunctionApproximatorBinaryBasis.hpp>

FunctionApproximatorBinaryBasis::FunctionApproximatorBinaryBasis(const Environment& env, const ConfigParser& cfg)
:Representation(env)
{
	Name = "Binary Basis Function Approximator";

	// Get the total number of actions
	// Total number of actions are needed to form phi and theata vectors.
	numberOfActions = env.number_of_actions;

	// Gradient Descent Learning Rate
	// Parameter denotes the learning rate of gradient descent optimization algorithm
	alpha = cfg.GetValueOfKey<double>("ALPHA",0.8);

	// Features are the locations radial basis functions.
	features = cfg.GetValueOfKey< vector<SmartVector> >("FEATURES");

	// Basis Functions
	//phi = SmartVector(numberOfFeatures*numberOfActions);
	//phi.Initialize();

	// Weights
	theta = SmartVector(features.size()*numberOfActions);
	theta.Initialize();

}

FunctionApproximatorBinaryBasis::~FunctionApproximatorBinaryBasis() {
	// TODO Auto-generated destructor stub
}

pair<int,double> FunctionApproximatorBinaryBasis::Get_Greedy_Pair(const SmartVector& state) const
{
	int maxArg;
	double maxVal = std::numeric_limits<double>::lowest();
	double temp = 0;

	vector <SmartVector> actions = environment->Get_Action_List(state);

	for (unsigned i = 0; i < actions.size(); ++i)
	{
		SmartVector action = actions[i];

		temp = Get_Value(state,action);

		if( temp > maxVal)
		{
			maxVal = temp;
			maxArg = i;
		}

	}

	return make_pair(maxArg,maxVal);
}

SmartVector FunctionApproximatorBinaryBasis::Get_Policy(const SmartVector& state) const
{
	SmartVector policy;

	return policy;
}

double FunctionApproximatorBinaryBasis::Get_Value(const SmartVector& state, const SmartVector& action) const
{
	SmartVector phi = Get_Phi(state,action);

	double QValue = SmartVector::InnerProduct(theta,phi);

	return QValue;
}

void FunctionApproximatorBinaryBasis::Set_Value(const SmartVector& state, const SmartVector& action, double value)
{
	// Calculate estimated Q Value
	double Qvalue = Get_Value(state,action);

	// Set Delta (Error = Q_plus - Qvalue)
	double delta = value - Qvalue;

	// Update Theta
	//SmartVector temp = alpha * delta * phi;
	theta = theta + alpha * delta * Get_Phi(state,action);

}

void FunctionApproximatorBinaryBasis::Print_Value()
{
	cout<<endl<<"Displaying State-Action Pair Q Value:"<<endl;


	vector <SmartVector> states = environment->Get_All_Possible_States();

	vector <SmartVector> actions;


	for (unsigned i = 0; i < states.size(); ++i) {

		cout<<"State #"<<i<< ":";

		actions = environment->Get_Action_List(states[i]);

		for (unsigned j = 0; j < actions.size(); ++j) {
			cout <<  setw(10) << setprecision(5) << Get_Value(states[i],actions[j]) << "   ";
		}
		cout<<endl;
	}
}

// Binary Phi
SmartVector FunctionApproximatorBinaryBasis::Get_Phi(const SmartVector& state,const SmartVector& action) const
{
	int feature_index = 0;

	SmartVector phi(features.size()*numberOfActions);

	// Set whole vector to zero
	phi.Initialize();

	for (unsigned i = 0; i < features.size(); ++i) {
		if(state==features[i])
		{
			// Get the right feature_index to set
			feature_index = i + (features.size() * action.index);

			// Check for invalid index
			assert(feature_index>=0 && feature_index< phi.size());

			// Set the related feature
			phi.elements[feature_index] = 1;

		}
	}

	return phi;
}

