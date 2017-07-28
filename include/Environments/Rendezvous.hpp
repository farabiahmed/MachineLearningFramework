/*
 * Rendezvous.hpp
 *
 *  Created on: Jul 26, 2017
 *      Author: farabiahmed
 */

#ifndef ENVIRONMENTS_RENDEZVOUS_HPP_
#define ENVIRONMENTS_RENDEZVOUS_HPP_

#include "Environments/Environment.hpp"
#include "Environments/Gridworld.hpp"
#include "Miscellaneous/ConfigParser.hpp"
#include "Representations/Representation.hpp"


class Rendezvous : public Gridworld
{
public:

	Rendezvous(const ConfigParser& cfg);

	~Rendezvous();

private:

	unsigned number_of_agents;

};

#endif /* ENVIRONMENTS_RENDEZVOUS_HPP_ */
