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

using namespace std;
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
	SmartVector Epsilon_Greedy_Policy(const SmartVector& state) const;

	// Show Trajectory for troubleshooting
	void Show_Trajectory(vector<pair<SmartVector,SmartVector>> trajectory) const;

protected:

	double gamma;

	double epsilon;

	double epsilonProbabilitySetValue;

	double epsilonProbability; 			/// Explore-Exploit Parameter.
										/// Probability that balances exploration and exploitation

	double epsilonProbabilityDecayRate; /// Explore-Exploit Parameter decay rate.

	unsigned int sample_length_L1;		/// Number of next state samples
	 	 	 	 	 	 	 	 	 	/// It will be used to approximate the expectations on the next state.

	unsigned int length_of_trajectory; 	/// Trajectory Length
										/// It will be used to determine the trajectory that the algorithm will follow for each iteration.
};

#endif /* AGENTS_TRAJECTORYBASEDVALUEITERATION_HPP_ */
