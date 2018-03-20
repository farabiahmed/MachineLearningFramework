/*
 * RepresentationDeepQNetworkUDP.cpp
 *
 *  Created on: Oct 9, 2017
 *      Author: farabiahmed
 */

#include <Representations/RepresentationDeepQNetworkUDP.hpp>

RepresentationDeepQNetworkUDP::RepresentationDeepQNetworkUDP(const Environment& env, const ConfigParser& cfg)
:RepresentationUDP(env,cfg)
{


}

RepresentationDeepQNetworkUDP::~RepresentationDeepQNetworkUDP() {

}

void RepresentationDeepQNetworkUDP::Add_Experience(const SmartVector& state, const SmartVector& action, const SmartVector& nextState, const double& reward, const int status)
{
	// Example Message for 2 agent:
	// experience,state,1,2,0,1,action,1,3,nextstate,2,2,3,1,reward,-1,status,5
	string data;
	data+="experience,";

	data+="state,";
	data+=Vector_ToString(state);
	data+=",";

	data+="action,";
	data+=Vector_ToString(action);
	data+=",";

	data+="nextstate,";
	data+=Vector_ToString(nextState);
	data+=",";

	data+="reward,";
	data+=to_string(reward);
	data+=",";

	data+="status,";
	data+=to_string(status);

	udpsocket.WriteSocket((unsigned char*)data.c_str(),data.size());


	// Return packet:
	// ex: OK,experience

	unsigned char rxBuffer[65535];
	int len = udpsocket.ReadSocket(rxBuffer,65535);
	//cout<<"UDP Receiver: "<<len<<" Data:"<<rxBuffer<<endl;
}

