/*
 * TrajectoryBasedValueIteration.hpp
 *
 *  Created on: Mar 22, 2017
 *      Author: farabiahmed
 */

#ifndef AGENTS_TRAJECTORYBASEDVALUEITERATIONMULTIAGENT_HPP_
#define AGENTS_TRAJECTORYBASEDVALUEITERATIONMULTIAGENT_HPP_

#include <cmath>
#include <iomanip>
#include <limits>       // std::numeric_limits
#include "Agents/Agent.hpp"
#include "Agents/TrajectoryBasedValueIteration.hpp"
#include "ProbabilityDistributions/ProbabilityDistribution.hpp"
#include "ProbabilityDistributions/DiscreteDistribution.hpp"
#include "Miscellaneous/ConfigParser.hpp"

using namespace std;
/*
 *
 */
class TrajectoryBasedValueIterationMultiAgent : public TrajectoryBasedValueIteration {
public:

	TrajectoryBasedValueIterationMultiAgent(const Environment* env, const Representation* rep, const ConfigParser& cfg)
	: TrajectoryBasedValueIteration(env, rep, cfg)
	{}

	// Solver
	virtual bool Start_Execution();
};

#endif /* AGENTS_TRAJECTORYBASEDVALUEITERATIONMULTIAGENT_HPP_ */
