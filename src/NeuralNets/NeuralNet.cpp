/*
 * NeuralNet.cpp
 *
 *  Created on: Apr 5, 2017
 *      Author: farabiahmed
 */

#include "NeuralNets/NeuralNet.hpp"

double NeuralNet::recentAverageSmoothingFactor = 100.0; // Number of training samples to average over

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
			layers.back().push_back(Neuron(numOutputs,neuronNum));

			cout<<"A neuron created"<<endl;
		}


        // Force the bias node's output to 1.0 (it was the last neuron pushed in this layer):
        layers.back().back().setOutputVal(1.0);

	}

	recentAverageError = 0;
	error = 0;
}

NeuralNet::~NeuralNet() {
	// TODO Auto-generated destructor stub
}

void NeuralNet::feedForward(const vector<double> &inputVals)
{
	// Check if number of input neurons is equal to inputs.
	// Otherwise throw runtime error.
	assert(inputVals.size()==layers[0].size()-1);

	// Assign the input values into the input neurons.
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

	error /= outputLayer.size() - 1;
	error = sqrt(error); // RMS

	// Implement a recent average measurement
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

		// Loop through each neurons of that layer
		for (unsigned neuronNum = 0; neuronNum < currentLayer.size(); ++neuronNum) {
			currentLayer[neuronNum].updateInputWeights(prevLayer);
		}

	}

}

void NeuralNet::getResults(vector<double> &resultVals) const
{
	resultVals.clear();

	for (unsigned neuronNum = 0; neuronNum < layers.back().size()-1; ++neuronNum) {
		resultVals.push_back(layers.back()[neuronNum].getOutputVal());
	}
}
