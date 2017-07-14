/*
 * NeuralNet.hpp
 *
 *  Created on: Apr 5, 2017
 *      Author: farabiahmed farabiahmed@gmail.com
 *      Thanks Dave Miller for NN tutorials.
 */

#ifndef NEURALNET_NEURALNET_HPP_
#define NEURALNET_NEURALNET_HPP_

#include <iostream>
#include <vector>
#include <iomanip>      // std::setprecision
#include <cassert> 		//assert
#include "NeuralNets/Neuron.hpp"

using namespace std;

// Forward Declaration for not implemented class
// class Neuron{};

typedef vector<Neuron> Layer;

/*
 *
 */
class NeuralNet {
public:
	NeuralNet(const vector<unsigned> &topology);
	virtual ~NeuralNet();
	void SetInitialWeights(const vector<vector<double>> &weights);
	void feedForward(const vector<double> &inputVals);
	void backPropagation(const vector<double> &targetVals);
	double LearnSequential(vector<vector<double>>& batchInput, vector<vector<double>>& batchLabel);
	double LearnBatch(vector<vector<double>>& batchInput, vector<vector<double>>& batchLabel);
	void getResults(vector<double> &resultVals) const;
	void getResults(unsigned layernum,vector<double> &resultVals) const;
	double getRecentAverageError(void) const { return recentAverageError; }
	int GetSizeOfInputLayer() const;
	int GetSizeOfOutputLayer() const;
	double GetWeight(int layer, int neuron, int connection) const;
	void Print() const;
	void showVectorVals(string label, vector<double> &v) const;

private:
	vector<Layer> layers; // layers[layerNum][neuronNum]
	double error; // training error

	// Some variables to track our training performance
	double recentAverageError;
	static double recentAverageSmoothingFactor;
};


#endif /* NEURALNET_NEURALNET_HPP_ */
