/*
 * Neuron.cpp
 *
 *  Created on: Apr 5, 2017
 *      Author: farabiahmed
 */

#include <NeuralNets/Neuron.hpp>



double Neuron::eta = 0.15; // Overall net learning rate, [0.0 ... 1.0] default: 0.15
double Neuron::alpha = 0.5; // Momentum, multiplier of last deltaWeight, [0.0 ... n]
Neuron::ACTIVATION_FUNCTION_TYPE Neuron::activation_function = Neuron::ACTIVATION_FUNCTION_DEFAULT;

Neuron::Neuron(unsigned numOutputs,unsigned index)
:myIndex(index)
{
	for (unsigned c = 0; c < numOutputs; ++c) {
		outputWeights.push_back(Connection());
		double temp = randomWeight();
		outputWeights.back().weight = temp;
		outputWeights.back().deltaWeight = 0;
	}
	outputVal = 0;
	gradient = 0;
}

Neuron::Neuron(unsigned numOutputs,unsigned index, vector<double> initialWeight)
:myIndex(index)
{
	for (unsigned c = 0; c < numOutputs; ++c) {
		outputWeights.push_back(Connection());
		outputWeights.back().weight = initialWeight[c];
		outputWeights.back().deltaWeight = 0;
	}
	outputVal = 0;
	gradient = 0;
}

void Neuron::SetOutputWeights(vector<double> initialWeight)
{
	for (unsigned c = 0; c < outputWeights.size(); ++c)
	{
		outputWeights[c].weight = initialWeight[c];
	}
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

	outputVal = Neuron::transferFunction(sum);
}

double Neuron::transferFunction(double x)
{
	if(activation_function==ACTIVATION_FUNCTION_TANH || activation_function==ACTIVATION_FUNCTION_DEFAULT)
	{
		// tanh - output range [-1 ... 1]
		return tanh(x);
	}
	else if(activation_function==ACTIVATION_FUNCTION_LRELU)
	{
		// reLu - output range [0 ... x]
		return (x<=0.0)?(0.5*x):x;
	}
	else if(activation_function==ACTIVATION_FUNCTION_RELU)
	{
		// reLu - output range [0 ... x]
		return (x<=0.0)?(0):x;
	}
	else if(activation_function==ACTIVATION_FUNCTION_SIGMOID)
	{
		// sigmoid
		return (1.0 / (1.0 + exp(-x)));
	}
	return 0;
}

double Neuron::transferFunctionDerivative(double x)
{
	if(activation_function==ACTIVATION_FUNCTION_TANH || activation_function==ACTIVATION_FUNCTION_DEFAULT)
	{
		// tanh derivative approximation
		// return 1.0 - x * x;
		double temp = tanh(x);
		return 1-temp*temp;

	}
	else if(activation_function==ACTIVATION_FUNCTION_LRELU)
	{
		// lreLu - output range [0 ... 1]
		return (x<=0.0)?0.5:1.0;
	}
	else if(activation_function==ACTIVATION_FUNCTION_RELU)
	{
		// reLu - output range [0 ... 1]
		return (x<=0.0)?0:1.0;
	}
	else if(activation_function==ACTIVATION_FUNCTION_SIGMOID)
	{
		// sigmoid
		double temp = (1.0 / (1.0 + exp(-x)));
		return (temp * (1 - temp));
	}
	return 0;
}

void Neuron::calcOutputGradients(double targetVal)
{
	double delta = targetVal - outputVal;
	gradient = - delta * Neuron::transferFunctionDerivative(outputVal);
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
	for (unsigned neuronNum = 0; neuronNum < prevLayer.size(); ++neuronNum)
	{
		Neuron &neuron = prevLayer[neuronNum];

		assert(myIndex>=0 && myIndex < neuron.outputWeights.size());

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

		// Do gradient descent subtraction, so we have updated the weight.
		neuron.outputWeights[myIndex].weight -= newDeltaWeight;

		// Store delta weight to calculate momentum later.
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
