/*
 * EnvironmentFactory.hpp
 *
 *  Created on: Apr 9, 2018
 *      Author: farabiahmed
 */

#ifndef ENVIRONMENTS_ENVIRONMENTFACTORY_HPP_
#define ENVIRONMENTS_ENVIRONMENTFACTORY_HPP_

#include "Environments/Environment.hpp"
#include "Environments/Gridworld.hpp"
#include "Environments/Blocksworld.hpp"
#include "Environments/Rendezvous.hpp"
#include "Miscellaneous/ConfigParser.hpp"

class EnvironmentFactory
{
public:

	static Environment* Create(string type, const ConfigParser& config)
	{
			if(type == "blocksworld") 		return new Blocksworld(config);
			else if(type == "gridworld")	return new Gridworld(config);
			else if(type == "rendezvous")	return new Rendezvous(config);
			else							return NULL;
	}
};



#endif /* ENVIRONMENTS_ENVIRONMENTFACTORY_HPP_ */
