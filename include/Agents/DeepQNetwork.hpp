/*
 * DeepQNetwork.hpp
 *
 *  Created on: Oct 9, 2017
 *      Author: farabiahmed
 */

#ifndef AGENTS_DEEPQNETWORK_HPP_
#define AGENTS_DEEPQNETWORK_HPP_

#include <Agents/TrajectoryBasedValueIteration.hpp>

/*
 *
 */
class DeepQNetwork: public TrajectoryBasedValueIteration {
public:
	DeepQNetwork(const Environment* env, const Representation* rep, const ConfigParser& cfg);
	virtual ~DeepQNetwork();

	// Solver
	bool Start_Execution();
};

#endif /* AGENTS_DEEPQNETWORK_HPP_ */
