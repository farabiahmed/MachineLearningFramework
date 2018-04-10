/*
 * AgentFactory.hpp
 *
 *  Created on: Apr 10, 2018
 *      Author: farabiahmed
 */

#ifndef AGENTS_AGENTFACTORY_HPP_
#define AGENTS_AGENTFACTORY_HPP_

#include "Agents/Agent.hpp"
#include "Agents/DeepQNetwork.hpp"
#include "Agents/GreedyAgent.hpp"
#include "Agents/OnlineTrajectoryValueIteration.hpp"
#include "Agents/QIteration.hpp"
#include "Agents/TrajectoryBasedValueIteration.hpp"

#include "Environments/Environment.hpp"
#include "Representations/Representation.hpp"
#include "Miscellaneous/ConfigParser.hpp"

class AgentFactory
{
public:

	static Agent* Create(string type, const Environment* env, const Representation* rep, const ConfigParser& cfg)
	{
			if(type == "deep-q-network") 							return new DeepQNetwork(env,rep,cfg);
			else if(type == "greedy-agent")							return new GreedyAgent(env,cfg);
			else if(type == "online-trajectory-value-iteration")	return new OnlineTrajectoryValueIteration(env,rep,cfg);
			else if(type == "q-iteration")							return new QIteration(env,rep,cfg);
			else if(type == "trajectory-based-value-iteration")		return new TrajectoryBasedValueIteration(env,rep,cfg);
			else													return NULL;
	}
};




#endif /* AGENTS_AGENTFACTORY_HPP_ */
