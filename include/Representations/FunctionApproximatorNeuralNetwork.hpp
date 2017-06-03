/*
 * FunctionApproximatorNeuralNetwork.hpp
 *
 *  Created on: May 9, 2017
 *      Author: farabiahmed
 */

#ifndef REPRESENTATIONS_FUNCTIONAPPROXIMATORNEURALNETWORK_HPP_
#define REPRESENTATIONS_FUNCTIONAPPROXIMATORNEURALNETWORK_HPP_

#include <cmath>
#include <iomanip>
#include <cassert>
#include "Environments/Environment.hpp"
#include "Representations/Representation.hpp"
#include "Miscellaneous/ConfigParser.hpp"
//#include "Tools/matplotlibcpp.h"
#include "NeuralNets/NeuralNet.hpp"
#include <cmath>


//namespace plt = matplotlibcpp;

/*
 *
 */
class FunctionApproximatorNeuralNetwork : public Representation {
public:
	FunctionApproximatorNeuralNetwork(const Environment& env, const ConfigParser& cfg);

	virtual ~FunctionApproximatorNeuralNetwork();

	pair<int,double> Get_Greedy_Pair(const SmartVector& state) const ;

	double Get_Value(const SmartVector& state, const SmartVector& action) const;

	void Set_Value(const SmartVector& state, const SmartVector& action, double value);

	SmartVector Get_Policy(const SmartVector& state) const;

	void Print_Value();

private:
	vector<double> errorList;

	vector<vector<double>> batch_features;
	vector<vector<double>> batch_labels;

	unsigned batchindex; 	/// It will be increased as every new sample received.
	 	 	 	 	 	 	/// Training process will start after reaching maxsize
	 	 	 	 	 	 	/// of batch samples (batch_samples.size())

	unsigned numberof_training_pass; /// It determines how many backprobagation training
	 	 	 	 	 	 	 	 	 /// will be done for current batch


protected:
	NeuralNet* network;
};

#endif /* REPRESENTATIONS_FUNCTIONAPPROXIMATORNEURALNETWORK_HPP_ */
