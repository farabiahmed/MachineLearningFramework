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

	enum Environment_Type
	{
		ENVIRONMENT_TYPE_NONE = 0,
		ENVIRONMENT_TYPE_BLOCKSWORLD,
		ENVIRONMENT_TYPE_GRIDWORLD,
		ENVIRONMENT_TYPE_RENDEZVOUS
	};

	static Environment* Create(Environment_Type type, const ConfigParser& config)
	{
		switch(type)
		{
			case ENVIRONMENT_TYPE_NONE:
				return NULL;
				break;
			case ENVIRONMENT_TYPE_BLOCKSWORLD:
				return new Blocksworld(config);
				break;
			case ENVIRONMENT_TYPE_GRIDWORLD:
				return new Gridworld(config);
				break;
			case ENVIRONMENT_TYPE_RENDEZVOUS:
				return new Rendezvous(config);
				break;
			default:
				return NULL;
				break;
		}

	}
};



#endif /* ENVIRONMENTS_ENVIRONMENTFACTORY_HPP_ */
