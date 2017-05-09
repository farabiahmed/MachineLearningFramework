//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <iostream>
#include <unordered_map>
#include "NeuralNets/NeuralNet.hpp"
#include "NeuralNets/Neuron.hpp"
#include "NeuralNets/TrainingData.hpp"

using namespace std;

void help_menu(void)
{
	cout << "NeuralNetwork Test...							"<<endl;
}

void showVectorVals(string label, vector<double> &v)
{
    cout << label << " ";
    for (unsigned i = 0; i < v.size(); ++i) {
        cout << v[i] << " ";
    }

    cout << endl;
}

int main()
{
	// Menu for user
	help_menu();

//	// Create a topology that holds layers and neurons
//	// e.g., {3,2,1}
//	vector<unsigned> topology;
//
//	// Instantiate topology
//	topology.push_back(3);
//	topology.push_back(2);
//	topology.push_back(1);
//
//	// Instantiate the neural network instance
//	NeuralNet myNet(topology);
//
//	vector<double> inputVals;
//	myNet.feedForward(inputVals);
//
//	vector<double> targetVals;
//	myNet.backPropagation(targetVals);
//
//	vector<double> resultVals;
//	myNet.getResults(resultVals);


	TrainingData trainData("config/trainingData.txt");

	// e.g., { 3, 2, 1 }
	vector<unsigned> topology;
	trainData.getTopology(topology);

	NeuralNet myNet(topology);

	vector<double> inputVals, targetVals, resultVals;
	int trainingPass = 0;

	while (!trainData.isEof()) {
		++trainingPass;
		cout << endl << "\033[32m Pass " << trainingPass << ": ";

		// Get new input data and feed it forward:
		if (trainData.getNextInputs(inputVals) != topology[0]) {
			cout<< endl << "End of Training Data" << endl;
			break;
		}

		showVectorVals("\033[33m Inputs:", inputVals);
		myNet.feedForward(inputVals);

		// Collect the net's actual output results:
		myNet.getResults(resultVals);
		showVectorVals("\033[34m Outputs:", resultVals);

		// Train the net what the outputs should have been:
		trainData.getTargetOutputs(targetVals);
		showVectorVals("Targets:", targetVals);
		assert(targetVals.size() == topology.back());

		myNet.backPropagation(targetVals);

		// Report how well the training is working, average over recent samples:
		cout << "\033[31m Net recent average error: "
				<< myNet.getRecentAverageError() << endl;

		//if(trainingPass>258)
		//	cout<<"test"<<endl;
	}

	cout << endl << "Training Done" << endl;

	cout << endl << "Testing Data" << endl;

	inputVals.clear();
	inputVals.push_back(0);
	inputVals.push_back(0);
	showVectorVals(": Inputs:", inputVals);
	myNet.feedForward(inputVals);

	myNet.getResults(resultVals);
	showVectorVals("Outputs:", resultVals);



	inputVals.clear();
	inputVals.push_back(0);
	inputVals.push_back(1);
	showVectorVals(": Inputs:", inputVals);
	myNet.feedForward(inputVals);

	myNet.getResults(resultVals);
	showVectorVals("Outputs:", resultVals);


	inputVals.clear();
	inputVals.push_back(1);
	inputVals.push_back(0);
	showVectorVals(": Inputs:", inputVals);
	myNet.feedForward(inputVals);

	myNet.getResults(resultVals);
	showVectorVals("Outputs:", resultVals);




	inputVals.clear();
	inputVals.push_back(1);
	inputVals.push_back(1);
	showVectorVals(": Inputs:", inputVals);
	myNet.feedForward(inputVals);

	myNet.getResults(resultVals);
	showVectorVals("Outputs:", resultVals);


	cout << endl << "Done" << endl;

	return 0;
}
