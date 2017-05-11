/*
 * Agent.hpp
 *
 *  Created on: Mar 20, 2017
 *      Author: farabiahmed
 */

#ifndef AGENTS_AGENT_HPP_
#define AGENTS_AGENT_HPP_

#include "Environments/Environment.hpp"
#include "Representations/Representation.hpp"

using namespace std;
/*
 *
 */
class Agent {
public:
	Agent();
	virtual ~Agent();
	virtual bool Start_Execution() = 0;

	virtual void Test(void) = 0;

	int max_number_of_iterations;

	string Name;	///< It stores the name of agent to increase readability and awareness.
    				///< Such as Qiteration, TrajectoryBased etc.



protected:
	Environment* environment;
	Representation* valueFunction;

};

#endif /* AGENTS_AGENT_HPP_ */
