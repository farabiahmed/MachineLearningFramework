/*
 * Representation.cpp
 *
 *  Created on: Mar 21, 2017
 *      Author: farabiahmed
 */

#include "Representations/Representation.hpp"
#include "Environments/Environment.hpp"

Representation::Representation(const Environment& env) {
	// TODO Auto-generated constructor stub
	environment = &env;
}

Representation::~Representation() {
	// TODO Auto-generated destructor stub
}

void Representation::Get_Report(string filePath, string fileName)
{
	// Create File Directory First
	system(("mkdir -p " + filePath).c_str());

	//Csv Output for pyton plots
	ofstream outfile(filePath+"/"+fileName);

	vector<SmartVector> states = environment->Get_All_Possible_States();

	for (unsigned i = 0; i < states.size(); ++i) {

		bool blocked_state = environment->Check_Blocked_State(states[i]);
		bool terminal_state = environment->Check_Terminal_State(states[i]);

		if( ! (blocked_state || terminal_state) )
		{
			vector<SmartVector> actions = environment->Get_Action_List(states[i]);

			for (unsigned a = 0; a < actions.size(); ++a)
			{
				for(int j=0; j< states[i].dimension; ++j)
				{
					outfile << (states[i].elements[j] - environment->state_mean[j])
							* environment->state_scalar[j]  << ",";
				}
				outfile <<  ( actions[a].elements[0] - environment->action_mean[0] ) * environment->action_scalar[0] << ",";
				outfile <<  Get_Value(states[i],actions[a]);
				outfile << endl;
			}
		}
	}
}
