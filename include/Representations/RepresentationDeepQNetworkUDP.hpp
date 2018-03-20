/*
 * RepresentationDeepQNetworkUDP.hpp
 *
 *  Created on: Oct 9, 2017
 *      Author: farabiahmed
 */

#ifndef REPRESENTATIONS_REPRESENTATIONDEEPQNETWORKUDP_HPP_
#define REPRESENTATIONS_REPRESENTATIONDEEPQNETWORKUDP_HPP_

#include <Representations/RepresentationUDP.hpp>

/*
 *
 */
class RepresentationDeepQNetworkUDP: public RepresentationUDP {
public:
	RepresentationDeepQNetworkUDP(const Environment& env, const ConfigParser& cfg);
	virtual ~RepresentationDeepQNetworkUDP();
	virtual void Add_Experience(const SmartVector& state, const SmartVector& action, const SmartVector& nextState, const double& reward, const int status);

};

#endif /* REPRESENTATIONS_REPRESENTATIONDEEPQNETWORKUDP_HPP_ */
