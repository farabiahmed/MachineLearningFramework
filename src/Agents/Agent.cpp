/*
 * Agent.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: farabiahmed
 */

#include "Agents/Agent.hpp"

Agent::Agent(const ConfigParser& cfg) : userControl(UserControl::GetInstance())
{
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

void Agent::Get_Cumulative_Rewards(unsigned numberof_bellmanupdate)
{
	cout<<endl<<endl<<"Cumulative Rewards Collecting: [";

	// Collect the performance of current session to draw a plot.
	vector<double> rewards;
	for (unsigned i = 0; i < number_of_simulations; ++i) {
		double r = Simulate();
		rewards.push_back(r);

		cout<<".";
	}

	auto pair = make_pair(numberof_bellmanupdate, rewards );

	// pair: <numberof_bellmanupdate, rewards_for_that_bellmanupdate>
	rewards_cumulative.push_back(pair);

	cout<<" OK]"<<endl<<endl;
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
	while( !environment->Check_Terminal_State(state) && number_of_movement<max_steps_in_simulation )
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

	// If simulation stucked and then terminated, warn user.
	if(number_of_movement>=max_steps_in_simulation) cout<<"x";

	return reward;
}

void Agent::Get_Report(string filePath, string fileName)
{
	cout<<endl<<"Creating agent report: "<<filePath<<"/"<<fileName<<endl;

	// Create File Directory First
	system(("mkdir -p " + filePath).c_str());

	//Csv Output for pyton plots
	ofstream logger(filePath+"/"+fileName);

	// Get the size of size of samples
	unsigned size_of_samples = rewards_cumulative[0].second.size();

	// Put Header first
	logger<<"update,";
	for (unsigned i = 0; i < size_of_samples; ++i)
	{
		logger<<"sample"<<i;
		if(i<size_of_samples-1)
			logger<<",";
	}
	logger<<endl;

	// Put data
	for (unsigned i = 0; i < rewards_cumulative.size(); ++i)
	{
		auto pair = rewards_cumulative[i];

		// Put the number that shows the bellman update number first
		logger<<pair.first<<",";

		// Then, put the rewards collected from simulation for that bellman update
		for (unsigned j = 0; j < pair.second.size(); ++j) {
			logger << pair.second[j];

			if( j < pair.second.size()-1)
				logger<<",";
		}
		logger<<endl;
	}
	logger.close();
}
