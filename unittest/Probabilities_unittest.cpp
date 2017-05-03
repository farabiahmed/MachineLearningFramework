//============================================================================
// Name        : main.cpp
// Author      : FarabiAhmed
// Version     :
// Copyright   :
// Description : C++, Ansi-style
//============================================================================

#include <iostream>
#include <ProbabilityDistributions/DiscreteDistribution.hpp>
#include <ProbabilityDistributions/ProbabilityDistribution.hpp>


using namespace std;

void help_menu(void)
{
	cout << "Probabilities Test...							"<<endl;
}

int main()
{
	// For Random Number Generator
	srand(time(0));

	// Menu for user
	help_menu();

	ProbabilityDistribution *p = nullptr;
	vector<double> prob = {0.1,0,0,0,0.5,0,0,0.4};

	delete p;
	p = new DiscreteDistribution(prob);


	vector<int> outcomes = p->Run_Experiment(100000);
	p->Print_Properties();

	for (unsigned int i = 0; i < outcomes.size(); ++i) {
		cout<<outcomes[i]<<endl;
	}

	return 0;
}
