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
#include "Tools/matplotlibcpp.h"
#include <cmath>

#define __cplusplus 201103L

namespace plt = matplotlibcpp;


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

	// Holds the errorlist
	vector<double> errorlist;
	vector<double> plot_xvalue;

	// Create a topology that holds layers and neurons
	// e.g., { 3, 2, 1 }
	vector<unsigned> topology;
	int numOfIteration=0;
	trainData.getConfig(topology,numOfIteration);

	NeuralNet myNet(topology);

	int numberOfNeurons=0;
	for(unsigned i=0; i<topology.size();i++)
	{
		numberOfNeurons+=(topology[i]+1);
	}

	vector<vector <double>> weightlist(numberOfNeurons);


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

		plot_xvalue.push_back(trainingPass);
		errorlist.push_back(myNet.getRecentAverageError());
		weightlist[0].push_back(myNet.GetWeight(0,0,0));
		weightlist[1].push_back(myNet.GetWeight(0,1,0));
		weightlist[2].push_back(myNet.GetWeight(0,2,0));
	}

	cout<<"\033[0m"<<endl; //Reset the color.

	cout << endl << "Training Done" << endl;

	cout << endl << "Testing..." << endl;



	for (unsigned i = 0; i < inputContainer.size(); ++i)
	{

		inputVals = inputContainer[i];

		showVectorVals("\033[33m Inputs:", inputVals);
		myNet.feedForward(inputVals);

		// Collect the net's actual output results:
		myNet.getResults(resultVals);
		showVectorVals("\033[34m Outputs:", resultVals);
	}

	myNet.Print();

     // Plot a line whose name will show up as "log(x)" in the legend.
     plt::named_plot("Recent Average Error", plot_xvalue, errorlist,"k");
     plt::named_plot("Weight", plot_xvalue, weightlist[0],"m");
     plt::named_plot("Weight", plot_xvalue, weightlist[1],"r");
     plt::named_plot("Weight", plot_xvalue, weightlist[2],"g");

     // Set x-axis to interval [0,1000000]
     plt::xlim(0, trainingPass);

     // Enable legend.
     plt::legend();

     // Show
     plt::show();

	cout << endl << "Done" << endl;

	return 0;
}
