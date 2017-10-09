/*
 * OnlineTrajectoryValueIteration.hpp
 *
 *  Created on: Sep 25, 2017
 *      Author: farabiahmed
 */

#ifndef AGENTS_ONLINETRAJECTORYVALUEITERATION_HPP_
#define AGENTS_ONLINETRAJECTORYVALUEITERATION_HPP_

#include <Agents/TrajectoryBasedValueIteration.hpp>
#include <fstream> // file write for datalog
#include <cstdlib> // system command

/*
 *
 */
class OnlineTrajectoryValueIteration: public TrajectoryBasedValueIteration {
public:
	OnlineTrajectoryValueIteration(const Environment* env, const Representation* rep, const ConfigParser& cfg);
	virtual ~OnlineTrajectoryValueIteration();

	// Solver
	bool Start_Execution();

};

#endif /* AGENTS_ONLINETRAJECTORYVALUEITERATION_HPP_ */
