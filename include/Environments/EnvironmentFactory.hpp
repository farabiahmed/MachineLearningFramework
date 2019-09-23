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
#include "Environments/Rendezvous_Refuel.hpp"
#include "Environments/DeliveryDrone.hpp"
#include "Miscellaneous/ConfigParser.hpp"

class EnvironmentFactory
{
public:

	static Environment* Create(string type, const ConfigParser& config)
	{
			if(type == "blocksworld") 				return new Blocksworld(config);
			else if(type == "gridworld")			return new Gridworld(config);
			else if(type == "rendezvous")			return new Rendezvous(config);
			else if(type == "rendezvous_refuel")	return new Rendezvous_Refuel(config);
			else if(type == "deliverydrone")		return new DeliveryDrone(config);
			else									return NULL;
	}
};



#endif /* ENVIRONMENTS_ENVIRONMENTFACTORY_HPP_ */
