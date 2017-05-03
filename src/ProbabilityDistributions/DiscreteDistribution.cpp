/*
 * DiscreteDistribution.cpp
 *
 *  Created on: Mar 10, 2017
 *      Author: farabiahmed
 */

#include "ProbabilityDistributions/DiscreteDistribution.hpp"


DiscreteDistribution::DiscreteDistribution(vector<double> probabilities) {

	Name = "Discrete Probability Distribution";
	this->probabilities = probabilities;
}

DiscreteDistribution::~DiscreteDistribution() {

}

/*!
   \brief The function that returns a list of outcomes.
   \param Number of Experiments
   \return List of outcomes
*/
vector<int> DiscreteDistribution::Run_Experiment(int number_of_experiments) {

	int size_of_probabilities = probabilities.size();	///< it will store number of random variables.

	vector<int> outcomes(size_of_probabilities, 0.0) ;	///< It stores the histogram of outputs.
														///< At the end it will return.

	double random_value = 0.0; 							///< It stores the generated random numbers locally.

	double cumulative_probability = 0.0;				///< It will store cumulative probability distribution.

	// Loop until we reach target number of experiments.
    for (int i = 0; i < number_of_experiments; i++) {

    	// Store current random value.
    	random_value = Generate_Random_Value();

    	// Reset cumulative probability for that expriment.
    	cumulative_probability = 0.0;

    	// Loop through our random variables to find a match
    	// between random value and our preloaded probabilities.
    	for (int j = 0; j < size_of_probabilities; ++j) {

    		cumulative_probability += probabilities[j];

    		if(random_value < cumulative_probability)
    		{
    			 outcomes[j]++;
    			 break;
    		}
		}

    }
    return outcomes;
}

void DiscreteDistribution::Print_Properties()
{
	cout << "Printing Probability Distribution Properties for "<< Name << endl;

	cout << "Discrete Probabilities are as follows: ";
    for (auto it = probabilities.begin(); it != probabilities.end(); ++it) {

		cout<< *it << "\t";
	}
    cout<<endl;
}
