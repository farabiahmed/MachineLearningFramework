/*
 * Rendezvous.cpp
 *
 *  Created on: Jul 26, 2017
 *      Author: farabiahmed
 */

#include <Environments/Rendezvous.hpp>

Rendezvous::Rendezvous(const ConfigParser& cfg)
:Gridworld(cfg)
{
	number_of_agents = cfg.GetValueOfKey<int>("NUMBER_OF_AGENTS");
}

Rendezvous::~Rendezvous()
{

}


