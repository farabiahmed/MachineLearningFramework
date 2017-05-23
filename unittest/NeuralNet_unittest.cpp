//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <iostream>
#include <unordered_map>
#include <iomanip>      // std::setprecision
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
        cout << std::setprecision(2) << v[i] << " ";
    }

    cout << endl;
}

int main(int argc, char* argv[])
{
	string filename = "";

	// Check the number of parameters
	if (argc < 2)
	{
		filename = "config/trainingData.txt";
	}
	else
	{
		filename = string(argv[1]);
	}

	// Menu for user
	help_menu();

	TrainingData trainData(filename);

	// Create a topology that holds layers and neurons
	// e.g., { 3, 2, 1 }
	vector<unsigned> topology;
	int numOfIteration=0;
	trainData.getConfig(topology,numOfIteration);

	NeuralNet myNet(topology);



	Neuron::alpha = 0.0;
	Neuron::eta = 0.01;
	Neuron::activation_function = Neuron::ACTIVATION_FUNCTION_LRELU;

	vector<vector<double>> inputContainer, targetContainer;
	vector<double> inputVals, targetVals, resultVals;

	while(!trainData.isEof())
	{
		// Get new input data and feed it forward:
		if (trainData.getNextInputs(inputVals) == topology[0])
		{
			// Load new input to related container
			inputContainer.push_back(inputVals);

			// Get related target to related container
			trainData.getTargetOutputs(targetVals);
			targetContainer.push_back(targetVals);

			// Check if the number of inputs and targets are equal.
			if(inputContainer.size() != targetContainer.size())
			{
				cout<<endl<<"Controlled abort due to missing data. Please check training data file:"
						<<filename<<endl<<endl;
				abort();
			}
		}
		else
		{
			cout<<endl<<"Controlled abort due to missing data. Please check training data file:"
					<<filename<<endl<<endl;
			abort();
		}
	}

	// Inform the user.
	cout<< endl << "End of Training Data" << endl;
	cout<< "Number Of Input Data: " << inputContainer.size()<<endl;

	int trainingPass = 0;

	while (trainingPass<numOfIteration)
	{
		++trainingPass;
		cout << endl << "\033[32m Pass " << trainingPass << ": ";

		inputVals = inputContainer[(trainingPass-1)%inputContainer.size()];

		showVectorVals("\033[33m Inputs:", inputVals);
		myNet.feedForward(inputVals);

		// Collect the net's actual output results:
		myNet.getResults(resultVals);
		showVectorVals("\033[34m Outputs:", resultVals);

		// Train the net what the outputs should have been:
		targetVals = targetContainer[(trainingPass-1)%targetContainer.size()];
		showVectorVals(" Targets:", targetVals);
		assert(targetVals.size() == topology.back());

		myNet.backPropagation(targetVals);

		// Report how well the training is working, average over recent samples:
		cout << "\033[31m Net recent average error: "
				<< myNet.getRecentAverageError() << endl;
	}

	cout<<"\033[0m"<<endl; //Reset the color.

	cout << endl << "Training Done" << endl;
/*
	cout << endl << "Testing Data" << endl;

	inputVals.clear();
	inputVals.push_back(0);
	inputVals.push_back(0);
	showVectorVals(": Inputs:", inputVals);
	myNet.feedForward(inputVals);

	myNet.getResults(resultVals);
	showVectorVals("Outputs:", resultVals);
*/

	cout << endl << "Done" << endl;

	return 0;
}
