/*
 * Neuron.hpp
 *
 *  Created on: Apr 5, 2017
 *      Author: farabiahmed
 */

#ifndef NEURALNETS_NEURON_HPP_
#define NEURALNETS_NEURON_HPP_

#include <vector>
#include <iostream>
#include <cstdlib> 		//rand
#include <cmath> 		//tanh

using namespace std;

// Forward Declaration for not implemented class
class Neuron;

typedef vector<Neuron> Layer;
/*
 *
 */
typedef struct
{
	double weight;
	double deltaWeight;
} Connection;

/*
 *
 */
class Neuron {

public:

	Neuron(unsigned numOutputs, unsigned index);

	virtual ~Neuron();

	void feedForward(const Layer &prevLayer);

	double getOutputVal(void) const { return outputVal; }

	void setOutputVal(double val) { outputVal = val; }

	void calcOutputGradients(double targetVal);

	void calcHiddenGradients(const Layer &nextLayer);

	void updateInputWeights(Layer &prevLayer);

	double sumDOW(const Layer &nextLayer) const;

private:

	// Static Methods

	static double randomWeight(void){	return rand() / double(RAND_MAX);	}

	static double transferFunction(double x);

	static double transferFunctionDerivative(double x);

	// Static Members (we want all the neurons can use the same value

	static double eta;	// [0.0 ... 1.0]
						// eta: overall net learning rate
						// 		0.0 - slow learner
						//		0.2 - medium learner
						//		1.0 - reckless learner

	static double alpha;// [0.0 ... n] multiplier of last weight change (momentum)
						// alpha: momentum
						//		0.0 - no momentum
						// 		0.5 - moderate momentum

	// Class Members
	double outputVal;

	vector<Connection> outputWeights;

	unsigned myIndex;

	double gradient;


};


#endif /* NEURALNETS_NEURON_HPP_ */
