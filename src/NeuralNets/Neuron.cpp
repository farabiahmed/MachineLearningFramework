/*
 * Neuron.cpp
 *
 *  Created on: Apr 5, 2017
 *      Author: farabiahmed
 */

#include <NeuralNets/Neuron.hpp>

double Neuron::eta = 0.15; // Overall net learning rate, [0.0 ... 1.0]
double Neuron::alpha = 0.5; // Momentum, multiplier of last deltaWeight, [0.0 ... n]

Neuron::Neuron(unsigned numOutputs,unsigned index) {

	for (unsigned c = 0; c < numOutputs; ++c) {
		outputWeights.push_back(Connection());
		outputWeights.back().weight = randomWeight();
		outputWeights.back().deltaWeight = 0;
	}

	this->myIndex = index;
}

Neuron::~Neuron() {
	// TODO Auto-generated destructor stub
}

void Neuron::feedForward(const Layer &prevLayer)
{
	double sum = 0.0;

	for (unsigned neuronNum = 0; neuronNum < prevLayer.size(); ++neuronNum) {
		sum += prevLayer[neuronNum].getOutputVal()
				* prevLayer[neuronNum].outputWeights[myIndex].weight;
	}

	outputVal = transferFunction(sum);
}

double Neuron::transferFunction(double x)
{
	// tanh - output range [-1 ... 1]
	return tanh(x);
}

double Neuron::transferFunctionDerivative(double x)
{
	// tanh derivative approximation
	return 1.0 - x * x;
}

void Neuron::calcOutputGradients(double targetVal)
{
	double delta = targetVal - outputVal;
	gradient = delta * Neuron::transferFunctionDerivative(outputVal);
}

void Neuron::calcHiddenGradients(const Layer &nextLayer)
{
	double dow = sumDOW(nextLayer);
	gradient = dow * Neuron::transferFunctionDerivative(outputVal);
}

void Neuron::updateInputWeights(Layer &prevLayer)
{
	// It will update the weights of the neurons of the previous layer
	// Weights are stored in Connection container.
	for (unsigned neuronNum = 0; neuronNum < prevLayer.size(); ++neuronNum) {

		Neuron &neuron = prevLayer[neuronNum];

		double oldDeltaWeight = neuron.outputWeights[myIndex].deltaWeight;

		// eta: overall net learning rate
		// 		0.0 - slow learner
		//		0.2 - medium learner
		//		1.0 - reckless learner

		// alpha: momentum
		//		0.0 - no momentum
		// 		0.5 - moderate momentum

		double newDeltaWeight =
				// Individual input, magnified by the gradient and train rate:
				eta
				* neuron.getOutputVal()
				* gradient
				// Also add momentum = a fraction of the previous delta weight
				+ alpha
				* oldDeltaWeight;

		neuron.outputWeights[myIndex].weight += newDeltaWeight;

		neuron.outputWeights[myIndex].deltaWeight = newDeltaWeight;
	}
}

double Neuron::sumDOW(const Layer &nextLayer) const
{
	double sum = 0.0;

	// Sum our contributions of the errors at the nodes we feed
	// Loop through neurons non including the bias neuron
	for (unsigned neuronNum = 0; neuronNum < nextLayer.size() - 1; ++neuronNum) {
		sum+=outputWeights[neuronNum].weight * nextLayer[neuronNum].gradient;
	}

	return sum;
}
