/*
 * RepresentationUDP.cpp
 *
 *  Created on: May 28, 2017
 *      Author: farabiahmed
 */

#include <Representations/RepresentationUDP.hpp>
#include <map>			// Pair of key-value
#include <string>
#include <iostream>
#include <exception>

using namespace std;

RepresentationUDP::RepresentationUDP(const Environment& env, const ConfigParser& cfg): Representation(env)
{
	Name = "Representation UDP";

	// Get the host ip
	host_ip = cfg.GetValueOfKey<string>("TARGET_HOST","127.0.0.1");

	// Get the receiver port
	port_rx = cfg.GetValueOfKey<int>("RECEIVE_PORT",4000);

	// Get the transmitter port
	port_tx = cfg.GetValueOfKey<int>("TRANSMIT_PORT",5000);

	// Initialize receiver
	udpsocket.Listen(port_rx);

	// Initialize transmitter
	udpsocket.SetTargetIp((char*)host_ip.c_str(),port_tx);

	gamma = cfg.GetValueOfKey<double>("GAMMA",0.8);


	// "config,NUMBER_OF_AGENTS=2,NUMBER_OF_ROWS=5,HIDDEN_LAYERS=12;6,ETA_LEARNING_RATE=0.1"
	string data;
	data+="config,";

	for (auto it : cfg.GetContents())
	{
		data += it.first + "=" + it.second + "|";
	}

	data.erase(data.size()-1,1);

	cout<<"Config Params:"<<endl;
	cout<<data<<endl;
	udpsocket.WriteSocket((unsigned char*)data.c_str(),data.size());


	// Return packet:
	// ex: OK,setvalue

	unsigned char rxBuffer[65535];
	int len = udpsocket.ReadSocket(rxBuffer,65535);
	//cout<<"UDP Receiver: "<<len<<" Data:"<<rxBuffer<<endl;
}

RepresentationUDP::~RepresentationUDP() {
	// TODO Auto-generated destructor stub
}

pair<int,double> RepresentationUDP::Get_Greedy_Pair(const SmartVector& state) const
{
	/*
	// Form the output vector.
	vector<double> resultVals;

	vector<SmartVector> actions = environment->Get_Action_List(state);

	for (unsigned i = 0; i < actions.size(); ++i) {
		resultVals.push_back( Get_Value(state,actions[i]) );
	}

	// initialize max value with minimum to update itself with the smallest bigger value.
	double maxVal = std::numeric_limits<double>::lowest();
	int maxArg=-1;

	// Get the index and value of maximum return
	for (unsigned i = 0; i < resultVals.size(); ++i) {
		if(resultVals[i] > maxVal)
		{
			maxVal = resultVals[i];
			maxArg = i;
		}
	}

	return make_pair(maxArg,maxVal);
	*/

	double maxVal = std::numeric_limits<double>::lowest();
	int maxArg=0;

	// Sending packet:
	// ex: getgreedypair,state,state.el[0],state.el[1],...,state.el[n]
	// ex: getgreedypair,state,2,3

	string data;
	data+="command,getgreedypair,";
	data+="state,";
	data+=Vector_ToString(state);

	udpsocket.WriteSocket((unsigned char*)data.c_str(),data.size());

	// Return packet:
	// ex: OK,getgreedypair,<arg>,<val>
	// ex: OK,getgreedypair,2,0.87

	unsigned char rxBuffer[65535]= {0};
	int len = udpsocket.ReadSocket(rxBuffer,65535);
	//cout<<"UDP Receiver: "<<len<<" Data:"<<rxBuffer<<endl;

	// Convert char array to string
	string str = string((char*)rxBuffer);

	// Parse string to vector-list
	vector<string> words = Convert::string_to_T< vector<string> >(str);

	assert(words[0] == "OK" && words[1] == "getgreedypair");

	try
	{
		maxArg = stoi(words[2]);
		maxVal = stod(words[3]);
	}
	catch (int e)
	{
		cout << "An exception occurred. Exception Code: " << e << '\n';
		cout << "Inputs: " << words[2] << " " << words[3] << '\n';

		exit(0);
	}

	return make_pair(maxArg,maxVal);
}


SmartVector RepresentationUDP::Get_Policy(const SmartVector& state) const
{
	// Get Best Policy Index
	pair<int,double> policyInfo = Get_Greedy_Pair(state);

	// Return It
	return environment->Get_Action_List(state)[policyInfo.first];
}

double RepresentationUDP::Get_Value(const SmartVector& state, const SmartVector& action) const
{
	// Sending packet:
	// ex: getvalue,state,state.el[0],state.el[1],...,state.el[n],action,action.el[0],..action.el[1]
	// ex: getvalue,state,2,3,action,3

	string data;
	data+="command,getvalue,";
	data+="state,";
	data+=Vector_ToString(state);
	data+=",";
	data+="action,";
	data+=Vector_ToString(action);

	udpsocket.WriteSocket((unsigned char*)data.c_str(),data.size());


	// Return packet:
	// ex: OK,getvalue,<val>
	// ex: OK,getvalue,0.87

	unsigned char rxBuffer[65535];
	int len = udpsocket.ReadSocket(rxBuffer,65535);
	//cout<<"UDP Receiver: "<<len<<" Data:"<<rxBuffer<<endl;

	// Convert char array to string
	string str((char*)rxBuffer);

	// Parse string to vector-list
	vector<string> words = Convert::string_to_T<vector<string>>(str);

	//cout<<"GetValue:" << str<<endl;
	assert(words[0] == "OK" && words[1] == "getvalue");
	try
	{
		return stod(words[2]);
	}
	catch (int e)
	{
		cout << "An exception occurred. Exception Code: " << e << '\n';
		cout << "Inputs: " << words[2] << " " << words[3] << '\n';

		exit(0);
		return 0;
	}
}

void RepresentationUDP::Set_Value(const SmartVector& state, const SmartVector& action, double value)
{
	// Sending packet:
	// ex: setvalue,value,<val>,state,state.el[0],state.el[1],...,state.el[n],action,action.el[0],..action.el[1]
	// ex: setvalue,valuestate,2,3,action,3

	string data;
	data+="command,setvalue,";
	data+="value,";
	data+=to_string(value);
	data+=",";
	data+="state,";
	data+=Vector_ToString(state);
	data+=",";
	data+="action,";
	data+=Vector_ToString(action);

	udpsocket.WriteSocket((unsigned char*)data.c_str(),data.size());


	// Return packet:
	// ex: OK,setvalue

	unsigned char rxBuffer[65535];
	int len = udpsocket.ReadSocket(rxBuffer,65535);
	//cout<<"UDP Receiver: "<<len<<" Data:"<<rxBuffer<<endl;
}

void RepresentationUDP::Add_Experience(const SmartVector& state, const SmartVector& action, const SmartVector& nextState, const double& reward, const int status)
{
	double maxQvalue = this->Get_Greedy_Pair(nextState).second;

	double qvalue = reward + gamma * maxQvalue;

	this->Set_Value(state,action,qvalue);
}
void RepresentationUDP::Print_Value()
{
	cout<<endl<<"Displaying State-Action Pair Q Value:"<<endl;

	vector<SmartVector> states = environment->Get_All_Possible_States();

	for (unsigned i = 0; i < states.size(); ++i) {

		cout<<"State #"<<i<< ":";

		auto actions = environment->Get_Action_List(states[i]);

		for (unsigned j = 0; j < actions.size(); ++j)
		{
			double QValue = Get_Value(states[i],actions[j]);

			cout <<  setw(10) << setprecision(5) << QValue << "   ";
		}
		cout<<endl;
	}
	cout<<endl;
}

string RepresentationUDP::Vector_ToString(const SmartVector& vec) const
{
	string s;

	for (int i = 0; i < vec.size(); ++i) {

		s+= std::to_string((int)vec.elements[i]);

		if(i!=vec.size()-1)
			s+=",";
	}

	return s;
}
