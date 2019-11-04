/*
 * RandomAgent.hpp
 *
 *  Created on: Nov 04, 2019
 *      Author: farabiahmed
 */

#ifndef AGENTS_RANDOMAGENT_HPP_
#define AGENTS_RANDOMAGENT_HPP_

#include <fstream> // file write for datalog
#include <cstdlib> // system command
#include <cmath>
#include <iomanip>
#include <limits>       // std::numeric_limits
#include "Agents/Agent.hpp"
#include "Environments/Environment.hpp"
#include "Representations/Representation.hpp"
#include "ProbabilityDistributions/ProbabilityDistribution.hpp"
#include "ProbabilityDistributions/DiscreteDistribution.hpp"
#include "Miscellaneous/UserControl.hpp"
#include "Miscellaneous/ConfigParser.hpp"

using namespace std;
/*
 *
 */
class RandomAgent : public Agent {
public:
	RandomAgent(const Environment* env, const ConfigParser& cfg);

	virtual ~RandomAgent();

	virtual bool Start_Execution(); ///< Main solver that runs the bellman iterations.

protected:

	unsigned int sample_length_L1;		/// Number of next state samples
	 	 	 	 	 	 	 	 	 	/// It will be used to approximate the expectations on the next state.

	unsigned int length_of_trajectory; 	/// Trajectory Length
										/// It will be used to determine the trajectory that the algorithm will follow for each iteration.

};

#endif /* AGENTS_RANDOMAGENT_HPP_ */
