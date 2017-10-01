/*
 * GreedyAgent.hpp
 *
 *  Created on: Sep 21, 2017
 *      Author: farabiahmed
 */

#ifndef AGENTS_GREEDYAGENT_HPP_
#define AGENTS_GREEDYAGENT_HPP_

#include <Agents/Agent.hpp>

/*
 *
 */
class GreedyAgent: public Agent {
public:
	GreedyAgent(const Environment* env, const ConfigParser& cfg);
	virtual ~GreedyAgent();

	bool Start_Execution();

	void PrintActions(const vector<SmartVector>& actions);
	void PrintRewards(const vector<double>& rewards);
};

#endif /* AGENTS_GREEDYAGENT_HPP_ */
