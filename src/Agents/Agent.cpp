/*
 * Agent.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: farabiahmed
 */

#include "Agents/Agent.hpp"

Agent::Agent() {

	/*
	// Inform the user about environment name.
	cout << "Environment Name: " << environment->Name << endl;

	// Inform the user about representation name.
	cout << "Representation Name: " << valueFunction->Name << endl;

	// Inform the user about representation name.
	cout << "Agent Name: " << Name << endl;
	*/
}

Agent::~Agent() {
	// TODO Auto-generated destructor stub
}

void Agent::Get_Cumulative_Rewards(void)
{
	// Collect the performance of iteration to draw a graphic.
	vector<double> reward_per_iteration;
	for (unsigned i = 0; i < number_of_simulations; ++i) {
		double r = Simulate();
		reward_per_iteration.push_back(r);
	}
	rewards_cumulative.push_back(reward_per_iteration);
}

double Agent::Simulate(void)
{
	// Hold the reward that we will return.
	double reward=0;

	// Count the number of total movement.
	unsigned number_of_movement = 0;

	// Current State of agent
	SmartVector state = environment->Get_Random_State();

	SmartVector nextState;
	SmartVector action;

	// Run the simulation and collect rewards.
	while( !environment->Check_Terminal_State(state) && number_of_movement<states.size() )
	{
		// Get next action to apply from updated policy
		action  = valueFunction->Get_Policy(state);

		// Get next state by iterating previous state and action.
		nextState = environment->Get_Next_State(state,action);

		// Sum the reward
		reward += environment->Get_Reward(state,action,nextState);

		// Move the agent to next state.
		state = nextState;

		// Increase total number of movement to be blocked in a loop in case of a bad policy.
		number_of_movement++;
	}

	return reward;
}

void Agent::Get_Report(string filePath, string fileName)
{
	// Create File Directory First
	system(("mkdir -p " + filePath).c_str());

	//Csv Output for pyton plots
	ofstream logger(filePath+"/"+fileName);

	// Put Header first
	for (unsigned i = 0; i < rewards_cumulative[0].size(); ++i)
	{
		logger<<"sample"<<i;
		if(i<rewards_cumulative[0].size()-1)
			logger<<",";
	}

	logger<<endl;

	// Put datas
	for (unsigned i = 0; i < rewards_cumulative.size(); ++i) {

		for (unsigned j = 0; j < rewards_cumulative[i].size(); ++j) {
			logger << rewards_cumulative[i][j];

			if( j < rewards_cumulative[i].size()-1)
				logger<<",";
		}
		logger<<endl;
	}
	logger.close();
}
