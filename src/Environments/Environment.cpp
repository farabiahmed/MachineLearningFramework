/*
 * environment.cpp
 *
 *  Created on: Mar 7, 2017
 *      Author: farabiahmed
 */

#include <Environments/Environment.hpp>

Environment::Environment() {
	// TODO Auto-generated constructor stub
	//cout<<"Class Environment Constructor"<<endl;

}

Environment::~Environment() {
	// TODO Auto-generated destructor stub
	//cout<<"Class Environment Destructor"<<endl;
}

SmartVector Environment::Get_Random_State()
{
	// Get All States
	vector<SmartVector> states = Get_All_Possible_States();

	// Current State of agent
	SmartVector state;

	// Random state index
	int state_index=-1;

	// Random value generator
	ProbabilityDistribution *p = new DiscreteDistribution(vector<double>(0));

	while( state_index==-1 || Check_Blocked_State(state) ||  Check_Terminal_State(state))
	{
		// Get a random state index.
		double randomval = p->Generate_Random_Value();

		state_index = (int) (randomval * (double)(states.size()-1));

		// Inform user
		//cout<<endl<<randomval<<"-"<<state_index<<endl;

		// Randomly Select Initial State.
		state = states[state_index];
	}

	delete p;
	return state;
}
