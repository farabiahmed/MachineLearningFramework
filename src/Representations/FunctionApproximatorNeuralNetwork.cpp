/*
 * FunctionApproximatorNeuralNetwork.cpp
 *
 *  Created on: May 9, 2017
 *      Author: farabiahmed
 */

#include <Representations/FunctionApproximatorNeuralNetwork.hpp>


FunctionApproximatorNeuralNetwork::FunctionApproximatorNeuralNetwork(const Environment& env, const ConfigParser& cfg)
:Representation(env)
{
	Name = "Neural Network Function Approximator";

	// Get the total number of states
	vector<SmartVector> states = env.Get_All_Possible_States();

	// Get the total number of actions
	vector<SmartVector> actions = env.Get_Action_List(states[0]);

	// Get the hidden layer information
	vector<SmartVector> hidden_layers = cfg.GetValueOfKey< vector<SmartVector> >("HIDDEN_LAYERS");

	// Get the learning rate for neural network
	Neuron::eta = cfg.GetValueOfKey<double>("ETA_LEARNING_RATE",0.15);

	// Batch size for learning multiple samples at a time for neural networks
	int batch_size = cfg.GetValueOfKey< int >("BATCH_SIZE",10);

	// Batch size for learning multiple samples at a time for neural networks
	numberof_training_pass = cfg.GetValueOfKey< int >("TRAINING_PASS_PER_BATCH",100);

	// e.g., { 3, 2, 1 }
	vector<unsigned> topology;

	// Input Layer: Total number of states are the number of input neurons.
	topology.push_back(states[0].dimension + actions[0].dimension);

	// Hidden Layers
	for (unsigned i = 0; i < hidden_layers.size(); ++i) {

		// Get the number of neurons for ith layer.
		unsigned number_of_neurons = hidden_layers[i].elements[0];

		// Load the given neurons to network topology.
		topology.push_back(number_of_neurons);
	}

	// Output Layer: is the actions.
	topology.push_back(1);

	// Create network model
	network = new NeuralNet(topology);

	// Create batch container, it will be loaded when set_value method called.
	batch_features = vector<vector<double>>(batch_size);
	for (unsigned i = 0; i < batch_features.size(); ++i)
	{
		// Each sample contains features first, then labels.
		batch_features[i] = vector<double>(network->GetSizeOfInputLayer());
	}
	batch_labels = vector<vector<double>>(batch_size);
	for (unsigned i = 0; i < batch_labels.size(); ++i)
	{
		// Each sample contains features first, then labels.
		batch_labels[i] = vector<double>(network->GetSizeOfOutputLayer());
	}

	// Initialize privates
	batchindex=0;

	network->Print();
}

FunctionApproximatorNeuralNetwork::~FunctionApproximatorNeuralNetwork() {

	// Clean
	delete network;
}

pair<int,double> FunctionApproximatorNeuralNetwork::Get_Greedy_Pair(const SmartVector& state) const
{
	// Form the output vector.
	vector<double> resultVals;

	vector<SmartVector> actions = environment->Get_Action_List(state);

	for (unsigned i = 0; i < actions.size(); ++i) {
		resultVals.push_back( Get_Value(state,actions[i]) );
	}

	// initialize max value with minimum to update itself with the smallest bigger value.
	double maxVal = std::numeric_limits<double>::lowest();
	int maxArg=-1;

	// Get the index and value of maximum return
	for (unsigned i = 0; i < resultVals.size(); ++i) {
		if(resultVals[i] > maxVal)
		{
			maxVal = resultVals[i];
			maxArg = i;
		}
	}

	return make_pair(maxArg,maxVal);
}

void FunctionApproximatorNeuralNetwork::Set_Value(const SmartVector& state, const SmartVector& action, double val)
{
	// Form the target/label vector.
	//vector<double> targetVals(network->GetSizeOfOutputLayer(),0);

	// Form the input vector.
	vector<double> inputVals(network->GetSizeOfInputLayer(),0);

	for (int i = 0; i < state.dimension; ++i) {
		inputVals[i] = (state.elements[i]  - environment->state_mean[i]) * environment->state_scalar[i];
	}
	for (int i = 0; i < action.dimension; ++i) {
		inputVals[ i + state.size() ] = (action.elements[i]  - environment->action_mean[i]) * environment->action_scalar[i];
	}

	// insert new samples and labels
	batch_features	[batchindex] 	= inputVals;
	batch_labels	[batchindex]	= vector<double>(network->GetSizeOfOutputLayer(),val);
	batchindex++;

	if(batchindex>=batch_features.size())
	{
		batchindex = 0;

		for (unsigned i = 0; i < numberof_training_pass; ++i) {

			for (unsigned j = 0; j < batch_features.size(); ++j)
			{
				// Feed forward the input:
				network->feedForward(batch_features[j]);

				// Train the net what the outputs that collected:
				network->backPropagation(batch_labels[j]);
			}
		}
	}

	// Report how well the training is working, average over recent samples:
	// cout <<endl<<"NetworkError: " << network->getRecentAverageError() <<endl;
	errorList.push_back(network->getRecentAverageError() );
}

double FunctionApproximatorNeuralNetwork::Get_Value(const SmartVector& state, const SmartVector& action) const
{
	// Form the input vector.
	vector<double> inputVals(network->GetSizeOfInputLayer(),0);

	// Form the output vector.
	vector<double> resultVals(network->GetSizeOfOutputLayer(),0);

	// Avoid the indexes that are out of limits.
	assert( (state.dimension + action.dimension) == (int)inputVals.size());
	assert( 1 == (int)resultVals.size());

	// TODO: Edit The comment
	// Fill the input data:
	// Our input vector is formed as follows:
	// input = {input_1, input_2, ..., input_n}
	// where 	n is the size of states,
	//			input_i is 1 if it is belong to that state,
	//			input_i is 0 otherwise.
	for (int i = 0; i < state.dimension; ++i) {
		inputVals[i] = (state.elements[i]  - environment->state_mean[i]) * environment->state_scalar[i];
	}

	for (int i = 0; i < action.dimension; ++i) {
		inputVals[ i + state.size() ] = (action.elements[i]  - environment->action_mean[i]) * environment->action_scalar[i];
	}

	// Feed forward the input:
	network->feedForward(inputVals);

	// Collect the net's actual output results:
	network->getResults(resultVals);

	// Return the requested value.
	return resultVals[0];
}

SmartVector FunctionApproximatorNeuralNetwork::Get_Policy(const SmartVector& state) const
{
	// Get Best Policy Index
	pair<int,double> policyInfo = Get_Greedy_Pair(state);

	// Return It
	return environment->Get_Action_List(state)[policyInfo.first];
}

void FunctionApproximatorNeuralNetwork::Print_Value()
{
	cout<<endl<<"Displaying State-Action Pair Q Value:"<<endl;

	// Form the input vector.
	vector<double> inputVals;

	// Form the output vector.
	vector<double> resultVals(network->GetSizeOfOutputLayer());

	vector<SmartVector> states = environment->Get_All_Possible_States();

	for (unsigned i = 0; i < states.size(); ++i) {

		cout<<"State #"<<i<< ":";

		auto actions = environment->Get_Action_List(states[i]);

		for (unsigned j = 0; j < actions.size(); ++j)
		{
			double QValue = Get_Value(states[i],actions[j]);

			cout <<  setw(10) << setprecision(5) << QValue << "   ";
		}
		cout<<endl;
	}
	cout<<endl;

    // Plot a line whose name will show up as "log(x)" in the legend.
    //plt::named_plot("Recent Average Error", errorList);

    // Set x-axis to interval [0,1000000]
    //plt::xlim(0, trainingPass);

    // Enable legend.
    //plt::legend();

    // Show
    //plt::show();
}
