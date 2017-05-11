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

	// e.g., { 3, 2, 1 }
	vector<unsigned> topology;

	// Input Layer: Total number of states are the number of input neurons.
	topology.push_back(states.size());

	// Hidden Layers
	for (unsigned i = 0; i < hidden_layers.size(); ++i) {

		// Get the number of neurons for ith layer.
		unsigned number_of_neurons = hidden_layers[i].elements[0];

		// Load the given neurons to network topology.
		topology.push_back(number_of_neurons);
	}

	// Output Layer: is the actions.
	topology.push_back(actions.size());

	// Create network model
	network = new NeuralNet(topology);
}

FunctionApproximatorNeuralNetwork::~FunctionApproximatorNeuralNetwork() {

	// Clean
	delete network;
}

pair<int,double> FunctionApproximatorNeuralNetwork::Get_Greedy_Pair(const SmartVector& state) const
{
	// Form the input vector.
	vector<double> inputVals(network->GetSizeOfInputLayer(),0);

	// Form the output vector.
	vector<double> resultVals(network->GetSizeOfOutputLayer(),0);

	// Avoid the indexes that are out of limits.
	assert(state.index	>=0 	&& 		state.index	< (int)inputVals.size());

	// Fill the input data:
	// Our input vector is formed as follows:
	// input = {input_1, input_2, ..., input_n}
	// where 	n is the size of states,
	//			input_i is 1 if it is belong to that state,
	//			input_i is 0 otherwise.
	inputVals[state.index] = 1.0;

	// Feed forward the input:
	network->feedForward(inputVals);

	// Collect the net's actual output results:
	network->getResults(resultVals);

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
	vector<double> targetVals(network->GetSizeOfOutputLayer(),0);

	// Form the input vector.
	vector<double> inputVals(network->GetSizeOfInputLayer(),0);
	inputVals[state.index] = 1.0;

	// Initialize training vector
	// Our training vector is formed as follows:
	// target = {target_1, target_2, ..., target_n}
	// where 	n is the size of actions,
	//			target_i is given-value 	:if it is belong to given action,
	//			target_i is existing-value	:otherwise.

	// In order to get the existing-value we need to feed-forward current
	// input to get other actions' QValue.

	// Feed forward the input:
	network->feedForward(inputVals);

	// Collect the net's actual output results to our targetVals variable:
	network->getResults(targetVals);

	// Modify the target with given new Q Value.
	targetVals[action.index] = val;

	// Train the net what the outputs should have been:
	network->backPropagation(targetVals);

	// Report how well the training is working, average over recent samples:
	//cout << endl<< "Error: " << network->getRecentAverageError() << endl;
}

double FunctionApproximatorNeuralNetwork::Get_Value(const SmartVector& state, const SmartVector& action) const
{
	// Form the input vector.
	vector<double> inputVals(network->GetSizeOfInputLayer(),0);

	// Form the output vector.
	vector<double> resultVals(network->GetSizeOfOutputLayer(),0);

	// Avoid the indexes that are out of limits.
	assert(state.index	>=0 	&& 		state.index	< (int)inputVals.size());
	assert(action.index	>=0 	&& 		action.index< (int)resultVals.size());

	// Fill the input data:
	// Our input vector is formed as follows:
	// input = {input_1, input_2, ..., input_n}
	// where 	n is the size of states,
	//			input_i is 1 if it is belong to that state,
	//			input_i is 0 otherwise.
	inputVals[state.index] = 1.0;

	// Feed forward the input:
	network->feedForward(inputVals);

	// Collect the net's actual output results:
	network->getResults(resultVals);

	// Return the requested value.
	return resultVals[action.index];
}

SmartVector FunctionApproximatorNeuralNetwork::Get_Policy(const SmartVector& state) const
{
	SmartVector vec;

	cout<<endl<<"NOT IMPLEMENTED YET"<<endl<<endl;

	return vec;
}

void FunctionApproximatorNeuralNetwork::Print_Value()
{
	cout<<endl<<"Displaying State-Action Pair Q Value:"<<endl;

	// Form the input vector.
	vector<double> inputVals;

	// Form the output vector.
	vector<double> resultVals(network->GetSizeOfOutputLayer());

	for (unsigned i = 0; i < (unsigned)network->GetSizeOfInputLayer(); ++i) {

		cout<<"State #"<<i<< ":";

		// Initialize the input with zeros than change the related one.
		inputVals = vector<double>(network->GetSizeOfInputLayer(),0);
		inputVals[i] = 1;

		// Feed forward the input:
		network->feedForward(inputVals);

		// Collect the net's actual output results:
		network->getResults(resultVals);

		for (unsigned j = 0; j < resultVals.size(); ++j)
		{
			double QValue = resultVals[j];

			cout <<  setw(10) << setprecision(5) << QValue << "   ";
		}
		cout<<endl;
	}
	cout<<endl;
}
