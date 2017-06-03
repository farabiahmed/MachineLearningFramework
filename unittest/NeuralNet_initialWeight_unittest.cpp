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
#include <string>     	// std::string, std::stod
#include <sstream>		// Conversion between string and primitives
#include "Miscellaneous/Convert.hpp"

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
vector<string> Parse_String(const string& val, const char& delimiter)
{
	vector<string> str_arr;

	string str = val;

	size_t pos = 0;

	while( (pos=str.find(delimiter)) !=  string::npos)
	{
		str_arr.push_back(str.substr(0,pos));
		str = str.erase(0,pos+1);
	}
	str_arr.push_back(str);

	return str_arr;
}


int main(int argc, char* argv[])
{
	string filename_trainingdata = "";
	string filename_weights = "";

	vector<vector<double>> weights;

	// Check the number of parameters
	if (argc < 3)
	{
		filename_trainingdata = "config/dataset_qvalue_mini.csv";
		filename_weights = "config/weights_tensorflow.csv";
	}
	else
	{
		filename_trainingdata = string(argv[1]);
		filename_weights = string(argv[2]);
	}

	// Menu for user
	help_menu();

	int numOfIteration=0;

	// Holds the errorlist
	vector<double> errorlist;
	vector<double> iterationlist, samplelist;

	Neuron::alpha = 0.0;
	Neuron::eta = 0.1;
	Neuron::activation_function = Neuron::ACTIVATION_FUNCTION_TANH;

	vector<vector<double>> inputContainer, targetContainer;
	vector<double> inputVals, targetVals, resultVals, estimatedVals;


	ifstream file ( filename_trainingdata );
	string value;
	while ( file.good() )
	{
	     getline ( file, value ); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
	     vector<string> tokens = Parse_String(value,',');

	     inputVals.clear();
	     for (unsigned i = 0; i < tokens.size()-1; ++i) {
	    	 inputVals.push_back((double)stod(tokens[i]));
	     }
		 inputContainer.push_back(inputVals);

	     targetVals.clear();
	     targetVals.push_back((double)stod(tokens.back()));
		 targetContainer.push_back(targetVals);
	}
	file.close();

	ifstream fileWeights ( filename_weights );
	while ( fileWeights.good() )
	{
	     getline ( fileWeights, value ); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/

	     vector<double> weight = Convert::string_to_T<vector<double>>(value);

	     weights.push_back(weight);
	}
	fileWeights.close();

	// Inform the user.
	cout<< endl << "End of Training Data" << endl;
	cout<< "Number Of Input Data: " << inputContainer.size()<<endl;


	// Create a topology that holds layers and neurons
	// e.g., { 3, 2, 1 }
	vector<unsigned> topology;
	topology.push_back(inputContainer[0].size());
	topology.push_back(4);
	topology.push_back(targetContainer[0].size());
	numOfIteration = 30000 * inputContainer.size();

	NeuralNet myNet(topology,weights);

	// Print Info
	myNet.Print();

	// Calculate total number of weights to store their value history.
	int numberOfWeights=0;
	for(unsigned i=0; i<topology.size()-1;i++)
	{
		unsigned numberOfNeurons = topology[i]+1;
		unsigned numberOfWeightsPerNeuron = topology[i+1];
		numberOfWeights+= numberOfNeurons * numberOfWeightsPerNeuron;
	}

	vector<vector <double>> weightlist(numberOfWeights);

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

		if((trainingPass%inputContainer.size()) == 0)
		{
			// Batch Training Completed

			// Print Info
			myNet.Print();

			if (trainingPass%1000)
			{
				iterationlist.push_back(trainingPass);
				errorlist.push_back(myNet.getRecentAverageError());
				// Get weights value history.
				int weightNumber=0;
				for(unsigned layer=0; layer<topology.size()-1;layer++)
				{
					for (unsigned  neuron = 0; neuron < topology[layer]+1; ++neuron)
					{
						for (unsigned  connection = 0; connection < topology[layer+1]; ++connection)
						{
							weightlist[weightNumber++].push_back(myNet.GetWeight(layer,neuron,connection));
							//cout<<layer<<"-"<<neuron<<"-"<<connection<<endl;
						}
					}
				}
			}
		}
	}

	cout<<"\033[0m"<<endl; //Reset the color.

	cout << endl << "Training Done" << endl;

	cout << endl << "Testing..." << endl;


	samplelist.clear();
	for (unsigned i = 0; i < inputContainer.size(); ++i)
	{

		inputVals = inputContainer[i];

		//showVectorVals("\033[33m Inputs:", inputVals);
		myNet.feedForward(inputVals);

		// Collect the net's actual output results:
		myNet.getResults(resultVals);
		//showVectorVals("\033[34m Outputs:", resultVals);

		estimatedVals.push_back(resultVals[0]);
		samplelist.push_back(i);
	}

	targetVals.clear();
	for (unsigned i = 0; i < targetContainer.size(); ++i) {
		targetVals.push_back(targetContainer[i][0]);
	}

	 //myNet.Print();

	 plt::figure();
	 plt::named_plot("Estimated", samplelist, estimatedVals,"b");
	 plt::named_plot("Target", samplelist, targetVals,"r");
     plt::xlim(0, (int)samplelist.back());
     plt::xlabel("Input: Tuple <state,action> id");
     plt::ylabel("Output: QValue");
     plt::title("Neural Network Target vs Estimated Values");
     plt::legend();

	 plt::figure();
     plt::named_plot("Recent Average Error", iterationlist, errorlist,"k");
     plt::xlim(0, trainingPass);
     plt::ylabel("Error");
     plt::xlabel("Iteration");
     plt::title("Neural Network Recent Average Error");
     plt::legend();


 	plt::figure();
 	for (unsigned i = 0; i < weightlist.size(); ++i) {
 		plt::named_plot("Weight"+to_string(i), iterationlist, weightlist[i]);
 	}
 	plt::ylabel("Weight Value");
 	plt::xlabel("Iteration");
 	plt::title("Neural Network Variation Of Weights During Training");

     // Show
     plt::show();

	cout << endl << "Done" << endl;

	return 0;
}


