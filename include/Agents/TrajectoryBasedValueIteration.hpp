/*
 * TrajectoryBasedValueIteration.hpp
 *
 *  Created on: Mar 22, 2017
 *      Author: farabiahmed
 */

#ifndef AGENTS_TRAJECTORYBASEDVALUEITERATION_HPP_
#define AGENTS_TRAJECTORYBASEDVALUEITERATION_HPP_

#include <cmath>
#include <iomanip>
#include <limits>       // std::numeric_limits
#include "Agents/Agent.hpp"
#include "ProbabilityDistributions/ProbabilityDistribution.hpp"
#include "ProbabilityDistributions/DiscreteDistribution.hpp"
#include "Miscellaneous/ConfigParser.hpp"

/*
 *
 */
class TrajectoryBasedValueIteration : public Agent {
public:

	// Constructor
	TrajectoryBasedValueIteration(const Environment* env, const Representation* rep, const ConfigParser& cfg);

	// Deconstructor
	virtual ~TrajectoryBasedValueIteration();

	// Solver
	bool Start_Execution();

	// Policy Selector
	SmartVector Epsilon_Greedy_Policy(const SmartVector& state);

	void Test(void);


	// MUST BE REMOVED TO ANOTHER REPRESENTATION CLASS
	SmartVector Get_Phi(const SmartVector& state,const SmartVector& action);

	double Get_Greedy_Value(const SmartVector& state,const vector<SmartVector>& actions);

private:

	double gamma;

	double epsilon;

	double epsilonProbability; 			/// Explore-Exploit Parameter.
										/// Probability that balances exploration and exploitation

	unsigned int sample_length_L1;		/// Number of next state samples
	 	 	 	 	 	 	 	 	 	/// It will be used to approximate the expectations on the next state.

	unsigned int length_of_trajectory; 	/// Trajectory Length
										/// It will be used to determine the trajectory that the algorithm will follow for each iteration.

	double alpha; 						/// Gradient Descent Learning Rate
										/// Parameter denotes the learning rate of gradient descent optimization algorithm

	int numberOfActions;

	int numberOfFeatures;

	SmartVector phi;

	SmartVector theta;
};

#endif /* AGENTS_TRAJECTORYBASEDVALUEITERATION_HPP_ */
