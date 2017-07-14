/*
 * NeuralNet.cpp
 *
 *  Created on: Apr 5, 2017
 *      Author: farabiahmed
 */

#include "NeuralNets/NeuralNet.hpp"

double NeuralNet::recentAverageSmoothingFactor = 1.0; // Number of training samples to average over

NeuralNet::NeuralNet(const vector<unsigned> &topology) {

	// Get the number of layers
	unsigned numLayers = topology.size();

	// Loop through desired number of layers and start to fill "layer" variable.
	for (unsigned layerNum = 0; layerNum < numLayers; ++layerNum) {

		// Add our new layer
		layers.push_back(Layer());

		// Get the number of outputs that a neuron should have in this layer
		// by knowing the neurons of the next layer.
		unsigned numOutputs = (layerNum < (numLayers - 1)) ? topology[layerNum+1] : 0;

		// Add neurons as indicated in topology[layerNum]
		// Don't forget to add bias neuron! That is why we used <= in loop.
		for (unsigned neuronNum = 0; neuronNum <= topology[layerNum]; ++neuronNum) {

			// Add neurons to our last created Layer
			// vector.back() is used to get latest layer.

			if(neuronNum == topology[layerNum])
			{
				// It is a bias neuron, then set its output-weights to zero.
				vector<double> weights(numOutputs,0);
				layers.back().push_back(Neuron(numOutputs,neuronNum,weights));
			}
			else
				// It is a normal neuron
				layers.back().push_back(Neuron(numOutputs,neuronNum));

			//cout<<"A neuron created"<<endl;
		}

        // Force the bias node's output to 1.0 (it was the last neuron pushed in this layer):
        layers.back().back().setOutputVal(1.0);
	}

	recentAverageError = -1;
	error = 0;
}

void NeuralNet::SetInitialWeights(const vector<vector<double>> &weights)
{
	// Get the number of layers
	unsigned numLayers = layers.size();

	unsigned weightsBiasOffset = numLayers - 1;

	// Loop through desired number of layers and start to fill "layer" variable.
	for (unsigned layerNum = 0; layerNum < numLayers; ++layerNum) {

		// Get the number of outputs that a neuron should have in this layer
		// by knowing the neurons of the next layer.
		unsigned numOutputs = (layerNum < (numLayers - 1)) ? layers[layerNum+1].size()-1 : 0;

		// Add neurons as indicated in topology[layerNum]
		// Don't forget to add bias neuron! That is why we used <= in loop.
		for (unsigned neuronNum = 0; neuronNum <= layers[layerNum].size()-1; ++neuronNum) {

			// Get the reference of current neuron that we want to set its weights
			Neuron &neuron = layers[layerNum][neuronNum];

			// It is output bias neuron. //TODO: Remove it -meaningless.
			if(neuronNum == layers[layerNum].size()-1 && layerNum == numLayers-1)
			{

			}
			// It is a bias neuron.
			else if(neuronNum == layers[layerNum].size()-1)
				neuron.SetOutputWeights(weights[weightsBiasOffset+layerNum]);

			// It is a weight neuron
			else
			{
				unsigned startIndex = neuronNum * numOutputs;
				unsigned stopIndex = startIndex + numOutputs;

				vector<double> weight(weights[layerNum].begin()+startIndex,	weights[layerNum].begin()+stopIndex);
				neuron.SetOutputWeights(weight);
			}
		}
	}

}

NeuralNet::~NeuralNet() {
	// TODO Auto-generated destructor stub
}

void NeuralNet::feedForward(const vector<double> &inputVals)
{
	// Check if number of input neurons is equal to inputs.
	// Otherwise throw runtime error.
	assert(inputVals.size()==layers[0].size()-1);

	// Assign (latch) the input values into the input neurons.
	for (unsigned i = 0; i < inputVals.size(); ++i) {
		layers[0][i].setOutputVal(inputVals[i]);
	}

	// Forward Propagation
	// Loop through all the layers to feed forward except input layer.
	for (unsigned layerNum = 1; layerNum < layers.size(); ++layerNum) {

		// Get previous layer to feed forward that neuron
		Layer &prevLayer = layers[layerNum-1];

		// Loop through all the neurons in that layer except bias neuron.
		for (unsigned neuronNum = 0; neuronNum < layers[layerNum].size() - 1; ++neuronNum) {
			layers[layerNum][neuronNum].feedForward(prevLayer);
		}
	}
}

void NeuralNet::backPropagation(const vector<double> &targetVals)
{
	// Calculate overall net error
	// This RMS function will be our
	// objective function for gradient descent algorithm.

	Layer &outputLayer = layers.back();
	error = 0.0;

	// RMS of output neuron errors
	// Loop through all the neurons in the output layer non including the bias neuron.
	for (unsigned neuronNum = 0; neuronNum < outputLayer.size() - 1; ++neuronNum) {

		// Get the delta value (error)
		double delta = targetVals[neuronNum] - outputLayer[neuronNum].getOutputVal();

		// Start to calculate RMS error
		error += delta * delta;
	}

	// Mean Squared Error (MSE)
	error /= (double)(outputLayer.size() - 1);

	// Root Mean Squared Error (RMSE)
	//error = sqrt(error);

	// Implement a recent average measurement

	// Initialize with first error.
	if( recentAverageError < 0)
		recentAverageError = error;
	else
		recentAverageError =
				(recentAverageError * recentAverageSmoothingFactor + error)
				/ (recentAverageSmoothingFactor + 1.0);

	// Calculate output layer gradients
	// Loop through all the neurons in the output layer non including the bias neuron.
	for (unsigned neuronNum = 0; neuronNum < outputLayer.size() - 1; ++neuronNum) {
		outputLayer[neuronNum].calcOutputGradients(targetVals[neuronNum]);
	}


	// Calculate gradients on hidden layers
	// Backwards loop: (outputLayer -> hiddenLayers -> inputLayer)
	// Sweeping from rightmost hidden layer (near the output layer) to leftmost hidden layer.
	// Not use input or output layer, just hidden layers!
	for (unsigned layerNum = layers.size() - 2; layerNum > 0; --layerNum) {
		Layer &hiddenLayer = layers[layerNum];
		Layer &nextLayer = layers[layerNum + 1];

		for (unsigned neuronNum = 0; neuronNum < hiddenLayer.size(); ++neuronNum) {
			hiddenLayer[neuronNum].calcHiddenGradients(nextLayer);
		}
	}



	// For all layers from outputs to first hidden layer,
	// update connection weights
	// Backwards loop: (outputLayer -> hiddenLayers -> inputLayer)
	// Starting from  rightmost layer (output-layer: layers.size() - 1)
	// to first hidden layer. No need to include input layer since
	// there is no weights coming to it from previous ones.
	for (unsigned layerNum = layers.size() - 1 ; layerNum > 0; --layerNum) {

		// For convenience and readability...
		Layer &currentLayer = layers[layerNum];
		Layer &prevLayer = layers[layerNum - 1];

		// Loop through each neurons of that layer except bias neuron
		for (unsigned neuronNum = 0; neuronNum < currentLayer.size()-1; ++neuronNum) {
			currentLayer[neuronNum].updateInputWeights(prevLayer);
		}

	}

}


double NeuralNet::LearnBatch(vector<vector<double>>& batchInput, vector<vector<double>>& batchLabel)
{
	// https://www.slideshare.net/MohammedBennamoun/neural-networks-multilayer-perceptron-backpropagation
	// p.74 Batch Update Formula.

	Layer &outputLayer = layers.back();
	vector<double> targetVals;

	// Store the gradients in SumOfGradients
	vector<vector<double>> SumOfGradients(layers.size());
	for (unsigned k = 0; k < SumOfGradients.size(); ++k) {
		for (unsigned n = 0; n < layers[k].size()-1; ++n)
		{
			SumOfGradients[k].push_back(0);
		}
	}


	for (unsigned i = 0; i < batchInput.size(); ++i) {

		feedForward(batchInput[i]);

		targetVals = batchLabel[i];

		// Calculate output layer gradients
		// Loop through all the neurons in the output layer non including the bias neuron.
		for (unsigned neuronNum = 0; neuronNum < outputLayer.size() - 1; ++neuronNum) {

			outputLayer[neuronNum].calcOutputGradients(targetVals[neuronNum]);

			SumOfGradients.back()[neuronNum] += outputLayer[neuronNum].GetGradient();
		}


		// Calculate gradients on hidden layers
		// Backwards loop: (outputLayer -> hiddenLayers -> inputLayer)
		// Sweeping from rightmost hidden layer (near the output layer) to leftmost hidden layer.
		// Not use input or output layer, just hidden layers!
		for (unsigned layerNum = layers.size() - 2; layerNum > 0; --layerNum) {
			Layer &hiddenLayer = layers[layerNum];
			Layer &nextLayer = layers[layerNum + 1];

			for (unsigned neuronNum = 0; neuronNum < hiddenLayer.size(); ++neuronNum) {
				hiddenLayer[neuronNum].calcHiddenGradients(nextLayer);

				SumOfGradients[layerNum][neuronNum] += hiddenLayer[neuronNum].GetGradient();
			}
		}
	}

	// Update the gradients at the end of batch.

	// For all layers from outputs to first hidden layer,
	// update connection weights
	// Backwards loop: (outputLayer -> hiddenLayers -> inputLayer)
	// Starting from  rightmost layer (output-layer: layers.size() - 1)
	// to first hidden layer. No need to include input layer since
	// there is no weights coming to it from previous ones.
	for (unsigned layerNum = layers.size() - 1 ; layerNum > 0; --layerNum) {

		// For convenience and readability...
		Layer &currentLayer = layers[layerNum];
		Layer &prevLayer = layers[layerNum - 1];

		// Loop through each neurons of that layer except bias neuron
		for (unsigned neuronNum = 0; neuronNum < currentLayer.size()-1; ++neuronNum) {

			currentLayer[neuronNum].SetGradient(SumOfGradients[layerNum][neuronNum]/batchInput.size());

			currentLayer[neuronNum].updateInputWeights(prevLayer);
		}
	}


	// Calculate overall net error
	// This RMS function will be our
	// objective function for gradient descent algorithm.

	error = 0.0;
	for (unsigned i = 0; i < batchInput.size(); ++i)
	{
		feedForward(batchInput[i]);
		targetVals = batchLabel[i];

		// RMS of output neuron errors
		// Loop through all the neurons in the output layer non including the bias neuron.
		for (unsigned neuronNum = 0; neuronNum < outputLayer.size() - 1; ++neuronNum) {

			// Get the delta value (error)
			double delta = targetVals[neuronNum] - outputLayer[neuronNum].getOutputVal();

			// Start to calculate RMS error
			error += delta * delta;
		}

		// Mean Squared Error (MSE)
		error /= (double)(outputLayer.size() - 1);

		// Root Mean Squared Error (RMSE)
		//error = sqrt(error);

		// Implement a recent average measurement

		// Initialize with first error.
		if( recentAverageError < 0)
			recentAverageError = error;
		else
			recentAverageError =
					(recentAverageError * recentAverageSmoothingFactor + error)
					/ (recentAverageSmoothingFactor + 1.0);
	}
	return error;
}

double NeuralNet::LearnSequential(vector<vector<double>>& batchInput, vector<vector<double>>& batchLabel)
{
	// Online Training
	double error = 0;

	vector<double> resultVals;

	for (unsigned i = 0; i < batchInput.size(); ++i)
	{
		//showVectorVals("\033[33m Inputs:", inputVals);
		feedForward(batchInput[i]);

		// Collect the net's hidden layer output results:
		//getResults(1,resultVals);
		//showVectorVals("L1 Outputs:", resultVals);

		// Collect the net's actual output results:
		//getResults(resultVals);
		//showVectorVals("Outputs:", resultVals);

		assert(batchLabel[i].size() == (unsigned)GetSizeOfOutputLayer());

		// Train the net what the outputs should have been:
		backPropagation(batchLabel[i]);

		//Print();

		// Report how well the training is working, average over recent samples:
		//cout << "\033[31m Net recent average error: "
		//		<< myNet.getRecentAverageError() << endl;

		error += getRecentAverageError();
	}
	error /= (double) batchInput.size();
	return error;
}

void NeuralNet::getResults(vector<double> &resultVals) const
{
	resultVals.clear();

	for (unsigned neuronNum = 0; neuronNum < layers.back().size()-1; ++neuronNum) {
		resultVals.push_back(layers.back()[neuronNum].getOutputVal());
	}
}

void NeuralNet::getResults(unsigned layernum,vector<double> &resultVals) const
{
	resultVals.clear();

	for (unsigned neuronNum = 0; neuronNum < layers[layernum].size()-1; ++neuronNum) {
		resultVals.push_back(layers[layernum][neuronNum].getOutputVal());
	}
}

int NeuralNet::GetSizeOfInputLayer() const
{
	return layers[0].size()-1;
}

int NeuralNet::GetSizeOfOutputLayer() const
{
	return layers.back().size()-1;
}

double NeuralNet::GetWeight(int layer, int neuron, int connection) const
{
	return layers[layer][neuron].outputWeights[connection].weight;
}
void NeuralNet::Print()const
{
	cout<<endl<<endl<<"Net Properties:"<<endl;
	cout<<"---------------------------------------------"<<endl;
	cout<<"Learning Rate\t:"<< Neuron::eta <<endl;
	cout<<"Momentum\t:"<< Neuron::alpha <<endl;
	cout<<"ActivationFunc\t:"<< Neuron::activation_function <<endl;
	cout<<"Topology\t:";
	for (unsigned i = 0; i <  layers.size(); ++i) {
		cout<<layers[i].size()-1<<" ";
	}
	cout<<endl;
	cout<<endl;

	cout<<"Initial Weights:"<<endl;
	cout<<"---------------------------------------------"<<endl;
	for (unsigned l = 0; l < layers.size(); ++l)
	{
		cout<<"Layer: "<<l<<endl;

		for (unsigned n = 0; n < layers[l].size(); ++n) {

			Neuron neuron = layers[l][n];

			cout<<"\tNeuron: " << n << "\tWeights: ";

			if(l==layers.size()-1)
				cout<<"None";
			else
				for (unsigned c = 0; c < neuron.outputWeights.size(); ++c) {
					cout<< setprecision(5) << neuron.outputWeights[c].weight<<" ";
				}

			cout<<endl;
		}
		cout<<endl;
	}
	cout<<endl;
}

void NeuralNet::showVectorVals(string label, vector<double> &v) const
{
    cout << label << " ";
    for (unsigned i = 0; i < v.size(); ++i) {
        cout << std::setprecision(2) << v[i] << " ";
    }

    cout << endl;
}
