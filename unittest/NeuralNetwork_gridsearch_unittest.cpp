//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <iostream>
#include <unordered_map>
#include <cstdlib> // system command
#include <iomanip>      // std::setprecision
#include "NeuralNets/NeuralNet.hpp"
#include "NeuralNets/Neuron.hpp"
#include "NeuralNets/TrainingData.hpp"
#include "Tools/matplotlibcpp.h"
#include "Miscellaneous/ConfigParser.hpp"
#include <cmath>
#include <string>     	// std::string, std::stod
#include <sstream>		// Conversion between string and primitives

#define __cplusplus 201103L

namespace plt = matplotlibcpp;


using namespace std;

// Prototypes
string Get_TimeStamp(void);
double NeuralNetwork_Run(unsigned maxiter, unsigned actfunc, double learnrate, double momentum, vector<unsigned> hiddenunits );
void Read_Training_Values();
void Print_Search_Parameters();
vector<string> Parse_String(const string& val, const char& delimiter);
void showVectorVals(string label, vector<double> &v);
void Create_Report(unsigned reportid);

// Global Variables
string filePath;

// GridSearch Related Variables:
string fileName;
vector<int> List_Iterations;
vector<int> List_ActivationFunctions ;
vector<double> List_LearningRates	;
vector<vector<unsigned>> List_HiddenUnits;
vector<double> List_Momentum;
vector<double> List_Result;

// Neural Network Related Variables:
vector<vector<double>> inputContainer, targetContainer;
vector<double> inputVals, targetVals, resultVals, estimatedVals;
vector<double> errorlist;
vector<double> iterationlist, samplelist;
vector<vector <double>> weightlist;

// General Use Variables:
string timestamp;

void help_menu(void)
{
	cout << "NeuralNetwork Test...							"<<endl;
}

int main(int argc, char* argv[])
{
	// Menu for user
	help_menu();

	// Get time for reporting
	timestamp = Get_TimeStamp();

	// Report File Directory:
	filePath = "log/" + timestamp;

	// Read GridSearch Configurations from config file
	ConfigParser cfg 		= ConfigParser("config/gridsearch_nn.cfg");
	fileName 				= cfg.GetValueOfKey<string>("TRAINING_DATA_FILE");
	List_Iterations 		= cfg.GetValueOfKey<vector<int>>("NUMBER_OF_ITERATIONS");
	List_ActivationFunctions= cfg.GetValueOfKey<vector<int>>("ACTIVATION_FUNCTIONS");
	List_LearningRates		= cfg.GetValueOfKey<vector<double>>("LEARNING_RATES");
	List_HiddenUnits		= cfg.GetValueOfKey<vector<vector<unsigned>>>("HIDDEN_UNITS");
	List_Momentum			= cfg.GetValueOfKey<vector<double>>("MOMENTUM_ALPHA");

	// Inform user about parameters before simulations
	Print_Search_Parameters();

	// Load the training values.
	Read_Training_Values();


	int search_number=0;
	for (unsigned i = 0; i < List_Iterations.size(); ++i) {
		for (unsigned j = 0; j < List_ActivationFunctions.size(); ++j) {
			for (unsigned k = 0; k < List_LearningRates.size(); ++k) {
				for (unsigned m = 0; m < List_Momentum.size(); ++m) {
					for (unsigned h = 0; h < List_HiddenUnits.size(); ++h) {


						cout<<endl<<"Grid Search Step: " <<search_number+1<<endl;
						cout<<"---------------------------------------------"<<endl;

						// Run Simulation with given parameters
						double average_error =
								NeuralNetwork_Run(
									List_Iterations[i],
									List_ActivationFunctions[j],
									List_LearningRates[k],
									List_Momentum[m],
									List_HiddenUnits[h]
													 );

						// Store the result to compare.
						List_Result.push_back(average_error);

						// Create detailed graphs to given path.
						Create_Report(search_number+1);

						search_number++;

					}
				}
			}
		}
	}

	// Inform user about parameters after simulations
	Print_Search_Parameters();

	cout << endl << "Done" << endl;

	return 0;
}


double NeuralNetwork_Run(unsigned maxiter, unsigned actfunc, double learnrate, double momentum, vector<unsigned> hiddenunits )
{
	// 1. Configuration: Iterations
	unsigned numOfIteration = maxiter;

	// 2. Configuration: Activation Function
	Neuron::activation_function = (Neuron::ACTIVATION_FUNCTION_TYPE)actfunc;

	// 3. Configuration: Learning Rate
	Neuron::eta = learnrate;

	// 4. Configuration: Momentum
	Neuron::alpha = momentum;

	// 5. Configuration: Hidden Units
	// Create a topology that holds layers and neurons
	// e.g., { 3, 2, 1 }
	vector<unsigned> topology;
	// Input Layer
	topology.push_back(inputContainer[0].size());
	// Hidden Layers
	for (unsigned i = 0; i < hiddenunits.size(); ++i) {
		topology.push_back(hiddenunits[i]);
	}
	// Output Layer
	topology.push_back(targetContainer[0].size());

	// Clear History
	iterationlist.clear();
	errorlist.clear();
	samplelist.clear();
	weightlist.clear();
	estimatedVals.clear();

	// Create Network
	NeuralNet myNet(topology);

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

	// Value history holder for each neuron.
	weightlist = vector<vector <double>>(numberOfWeights);

	// Training Pass Holder to not miss given iterations
	unsigned trainingPass = 0;

	// Iteration loop
	while (trainingPass<numOfIteration)
	{
		++trainingPass;
		//cout << endl << "\033[32m Pass " << trainingPass << ": ";

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
		//cout << "\033[31m Net recent average error: "
		//		<< myNet.getRecentAverageError() << "\033[0m"<< endl;

		// Hold iteration history to draw nice plots.
		iterationlist.push_back(trainingPass);

		// Hold recent error history to draw plot.
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

	cout<<"\033[0m"<<endl; //Reset the color.

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

	return myNet.getRecentAverageError();
}

string Get_TimeStamp(void)
{
	time_t now = time(NULL);
	tm * timeinfo = localtime(&now);
	char buffer[80];
	strftime(buffer, 80, "%Y%m%d_%H%M%S", timeinfo);
	return string(buffer);
}

void showVectorVals(string label, vector<double> &v)
{
    //cout << label << " ";
    for (unsigned i = 0; i < v.size(); ++i) {
        //cout << std::setprecision(2) << v[i] << " ";
    }

    //cout << endl;
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

void Print_Search_Parameters()
{
	cout<<endl<<"Grid Search Parameters:"<<endl;
	cout<<"FileName:"<<fileName<<endl;
	cout<<"Search\t";
	cout<<"Iteration\t";
	cout<<"Activation\t";
	cout<<"LearningRate\t";
	cout<<"Momentum\t";
	cout<<"HiddenUnits\t";
	cout<<"Result\t";
	cout<<endl;

	int search_number=0;
	for (unsigned i = 0; i < List_Iterations.size(); ++i) {
		for (unsigned j = 0; j < List_ActivationFunctions.size(); ++j) {
			for (unsigned k = 0; k < List_LearningRates.size(); ++k) {
				for (unsigned m = 0; m < List_Momentum.size(); ++m) {
					for (unsigned h = 0; h < List_HiddenUnits.size(); ++h) {

						cout<<++search_number<<".\t";
						cout<<List_Iterations[i]<<"\t\t";
						cout<<List_ActivationFunctions[j]<<"\t\t";
						cout<<List_LearningRates[k]<<"\t\t";
						cout<<List_Momentum[m]<<"\t\t";

						for (unsigned g = 0; g < List_HiddenUnits[h].size(); ++g) {
							cout<<List_HiddenUnits[h][g]<<" ";
						}

						cout<<"\t\t";
						if(List_Result.size())
							cout<<List_Result[search_number-1];
						else
							cout<<"-";

						cout<<endl;
					}
				}
			}
		}
	}

	cout<<endl;
}

void Read_Training_Values()
{
	cout<<endl<<"Loading Training Values...";
	ifstream file ( fileName );
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

	cout<<"OK"<<endl<<endl;
}
void Create_Report(unsigned reportid)
{
	// Convert report id to string
	string reportstr = to_string(reportid);

	// Create File Directory First
	system(("mkdir -p " + filePath).c_str());

	plt::figure();
	plt::named_plot("Estimated", samplelist, estimatedVals,"b");
	plt::named_plot("Target", samplelist, targetVals,"r");
	plt::xlabel("Input: Tuple <state,action> id");
	plt::ylabel("Output: QValue");
	plt::title("Neural Network Target vs Estimated Values");
	plt::legend();
	plt::save(filePath+"/targetvsestimated"+reportstr+".svg");
	plt::clf();

	plt::figure();
	plt::named_plot("Recent Average Error", iterationlist, errorlist,"k");
	plt::ylabel("Error");
	plt::xlabel("Iteration");
	plt::title("Neural Network Recent Average Error");
	plt::legend();
	plt::save(filePath+"/error"+reportstr+".svg");
	plt::clf();

	plt::figure();
	for (unsigned i = 0; i < weightlist.size(); ++i) {
		plt::named_plot("Weight"+to_string(i), iterationlist, weightlist[i]);
	}
	plt::ylabel("Weight Value");
	plt::xlabel("Iteration");
	plt::title("Neural Network Variation Of Weights During Training");
	//plt::legend();
	plt::save(filePath+"/weights"+reportstr+".svg");
	plt::clf();
}
