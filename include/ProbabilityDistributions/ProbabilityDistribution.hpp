/*
 * ProbabilityDistribution.h
 *
 *  Created on: Mar 10, 2017
 *      Author: farabiahmed
 */

#ifndef PROBABILITYDISTRIBUTIONS_PROBABILITYDISTRIBUTION_H_
#define PROBABILITYDISTRIBUTIONS_PROBABILITYDISTRIBUTION_H_

#include <vector>
#include <ctime>
#include <cstdlib> //srand() and rand()
#include <time.h>  //time(0)
#include <string>

using namespace std;

/*
 *
 */
class ProbabilityDistribution {

public:

	// Constructor
	ProbabilityDistribution();

	// Virtual Destructor
	virtual ~ProbabilityDistribution();

	// Random Value Generator between 0 and 1
	virtual double Generate_Random_Value();

	/*!
	   \brief The function that returns a list of outcomes.
	   \param Number of Experiments
	   \return List of outcomes
	*/
	virtual vector<int> Run_Experiment(int number_of_experiments) = 0;

	// Print out Properties of the Distribution
	virtual void Print_Properties() = 0;

	string Name;	///< It stores the name of environment to increase readability and awareness.
    				///< Such as Discrete Probability Distribution, etc.
};

#endif /* PROBABILITYDISTRIBUTIONS_PROBABILITYDISTRIBUTION_H_ */
