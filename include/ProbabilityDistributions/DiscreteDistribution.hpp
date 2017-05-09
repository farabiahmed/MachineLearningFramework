/*
 * DiscreteDistribution.h
 *
 *  Created on: Mar 10, 2017
 *      Author: farabiahmed
 */

#ifndef PROBABILITYDISTRIBUTIONS_DISCRETEDISTRIBUTION_H_
#define PROBABILITYDISTRIBUTIONS_DISCRETEDISTRIBUTION_H_

#include <vector>
#include <iostream>
#include "ProbabilityDistributions/ProbabilityDistribution.hpp"

using namespace std;

/*
 * Example Usage
 */

/*
	vector<double> prob = {0.2, 0.3, 0.5};
	ProbabilityDistribution *p = new DiscreteDistribution(prob);
	vector<int> outcomes = p->Run_Experiment(1000);
	p->Print_Properties();

	ofstream csvFile;
    csvFile.open ("../doc/latex/histogram.csv");
    csvFile << "probability,outcome\n";

    for (unsigned int i = 0; i < outcomes.size(); ++i) {
		cout<< outcomes[i] << "\t";
		csvFile<< prob[i] << "," << outcomes[i] << endl;
	}
    cout<<endl;

    csvFile.close();
 */

class DiscreteDistribution : public ProbabilityDistribution {
public:
	//Constructor
	DiscreteDistribution(vector<double> prob);

	//Destructor
	virtual ~DiscreteDistribution();

	//Experiment Function
	vector<int> Run_Experiment(int);

	//Print Properties of this class
	void Print_Properties();

private:
	vector<double> probabilities;		///< It stores discrete probabilities of variables
										///< Such as {0.2, 0.8} or {0.1, 0.65, 0.15} etc.
};

#endif /* PROBABILITYDISTRIBUTIONS_DISCRETEDISTRIBUTION_H_ */
