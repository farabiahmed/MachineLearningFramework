/*
 * RepresentationFactory.hpp
 *
 *  Created on: Apr 10, 2018
 *      Author: farabiahmed
 */

#ifndef REPRESENTATIONS_REPRESENTATIONFACTORY_HPP_
#define REPRESENTATIONS_REPRESENTATIONFACTORY_HPP_

#include <Representations/RepresentationActorCriticTabular.hpp>
#include <Representations/TabularStateActionPair.hpp>
#include "Representations/FunctionApproximatorNeuralNetwork.hpp"
#include "Representations/RepresentationDeepQNetworkUDP.hpp"
#include "Representations/RepresentationUDP.hpp"
#include "Representations/RepresentationEmpty.hpp"
#include "Environments/Environment.hpp"
#include "Representations/Representation.hpp"
#include "Miscellaneous/ConfigParser.hpp"

class RepresentationFactory
{
public:

	static Representation* Create(string type, const Environment* env, const ConfigParser& cfg)
	{
			if(type == "FunctionApproximatorNeuralNetwork") 		return new FunctionApproximatorNeuralNetwork(*env,cfg);
			else if(type == "RepresentationDeepQNetworkUDP")		return new RepresentationDeepQNetworkUDP(*env,cfg);
			else if(type == "RepresentationUDP")					return new RepresentationUDP(*env,cfg);
			else if(type == "TabularStateActionPair")				return new TabularStateActionPair(*env,cfg);
			else if(type == "RepresentationActorCriticTabular")		return new RepresentationActorCriticTabular(*env,cfg);
			else if(type == "RepresentationEmpty")					return new RepresentationEmpty(*env,cfg);
			else													return NULL;
	}
};





#endif /* REPRESENTATIONS_REPRESENTATIONFACTORY_HPP_ */
