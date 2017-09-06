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

	// Check the number of parameters given.
	if(argc == 1)
	{
		// Ex:
		// ./bin/NeuralNet_initialWeight_unittest

		filename_trainingdata = "config/dataset_qvalue_mini.csv";
		filename_weights = "config/weights_tensorflow.csv";
	}
	else if (argc == 2)
	{
		// Ex:
		// ./bin/NeuralNet_initialWeight_unittest config/dataset_qvalue_mini.csv

		filename_trainingdata = string(argv[1]);
		filename_weights = "";
	}
	else
	{
		// Ex:
		// ./bin/NeuralNet_initialWeight_unittest config/dataset_qvalue_mini.csv config/weights_tensorflow.csv

		filename_trainingdata = string(argv[1]);
		filename_weights = string(argv[2]);
	}

	// Menu for user
	help_menu();

	// Some lists to hold plotting related variables.
	vector<double> errorlist;
	vector<double> iterationlist, samplelist;

	// Batch samples of input-output lists.
	vector<vector<double>> batchInput, batchLabel;

	// Temporary variables
	vector<double> inputVals, targetVals, resultVals, estimatedVals;


	// Set the neuron parameters.
	Neuron::alpha = 0.0;
	Neuron::eta = 0.1;
	Neuron::activation_function = Neuron::ACTIVATION_FUNCTION_TANH;

	// Set the number of iterations
	int numOfIteration = 3000;

	// Read Training Data File and load them to batchInput and batchLabel variables
	ifstream file ( filename_trainingdata );
	string value;
	int line_no=0;
	while ( file.good() )
	{
		line_no++;
		getline ( file, value ); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/

		if(!value.empty())
		{
			cout<<"Reading line: "<<line_no<<" Content: "<<value<<endl;
			vector<string> tokens = Parse_String(value,',');

			inputVals.clear();
			for (unsigned i = 0; i < tokens.size()-1; ++i) {
			inputVals.push_back((double)stod(tokens[i]));
			}
			batchInput.push_back(inputVals);

			targetVals.clear();
			targetVals.push_back((double)stod(tokens.back()));
			batchLabel.push_back(targetVals);
		}
	}
	file.close();

	// If initial weights are given load them to weights container.
	if(filename_weights!="")
	{
		ifstream fileWeights ( filename_weights );
		while ( fileWeights.good() )
		{
			 getline ( fileWeights, value ); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/

			 if(value.size()>0)
			 {
				 vector<double> weight = Convert::string_to_T<vector<double>>(value);
				 weights.push_back(weight);
			 }
		}
		fileWeights.close();
	}


	// Inform the user.
	cout<< endl << "End of Fetching Training Data" << endl;

	cout<< endl << "Number Of Input Data: " << batchInput.size()<<endl;

	// Create a topology that holds layers and neurons
	vector<unsigned> topology;
	// Input units
	topology.push_back(batchInput[0].size());
	// Hidden units
	topology.push_back(24);
	// Output Units
	topology.push_back(batchLabel[0].size());

	NeuralNet myNet(topology);

	if(weights.size()>0)
		myNet.SetInitialWeights(weights);

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

	// Main Training Loop
	while (trainingPass<numOfIteration)
	{
		++trainingPass;
		cout << endl << "\033[32m Pass " << trainingPass << ": ";

		double error=0;
		error = myNet.LearnSequential(batchInput,batchLabel);
		//error = myNet.LearnBatch(batchInput,batchLabel);
		cout<<"Error: "<< error;

		if (trainingPass%1000)
		{
			iterationlist.push_back(trainingPass);
			errorlist.push_back(error);
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

	cout<<"\033[0m"<<endl; //Reset the color.

	cout << endl << "Training Done" << endl;

	cout << endl << "Testing..." << endl;

	samplelist.clear();
	for (unsigned i = 0; i < batchInput.size(); ++i)
	{

		inputVals = batchInput[i];

		//showVectorVals("\033[33m Inputs:", inputVals);
		myNet.feedForward(inputVals);

		// Collect the net's actual output results:
		myNet.getResults(resultVals);
		//showVectorVals("\033[34m Outputs:", resultVals);

		estimatedVals.push_back(resultVals[0]);
		samplelist.push_back(i);
	}

	targetVals.clear();
	for (unsigned i = 0; i < batchLabel.size(); ++i) {
		targetVals.push_back(batchLabel[i][0]);
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
    //plt::subplot(2,2,1);
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


