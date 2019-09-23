/*
 * Agent.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: farabiahmed
 */

#include "Agents/Agent.hpp"
#include <iomanip>

Agent::Agent(const ConfigParser& cfg) :
userControl(UserControl::GetInstance())
{
	/*
	// Inform the user about environment name.
	cout << "Environment Name: " << environment->Name << endl;

	// Inform the user about representation name.
	cout << "Representation Name: " << valueFunction->Name << endl;

	// Inform the user about representation name.
	cout << "Agent Name: " << Name << endl;
	*/

	log_file_path = "log/"+ cfg.GetValueOfKey<string>("TIME_STAMP");

	// Log parameters
	// Create File Directory First
	system(("mkdir -p " + log_file_path).c_str());

	logger.open((log_file_path + "/agentReport2.csv").c_str(),fstream::out);

}

Agent::~Agent() {
	// TODO Auto-generated destructor stub
}

void Agent::Get_Cumulative_Rewards(unsigned numberof_bellmanupdate)
{
	static int game_played;
	vector<double> rewards;

	//cout<<endl<<endl<<"Game Simulation: #"<< game_played++;
	cout<<" Sim: #"<< setw(6) << game_played++;

	// Collect the performance of current session to draw a plot.
	vector<pair<int, double>> sim_results;
	for (unsigned i = 0; i < number_of_simulations; ++i) {
		pair<unsigned, double> sim = Simulate();

		sim_results.push_back(sim);
	}

	cout<<" Results:[ ";
	for(auto sim:sim_results)
	{
		if((unsigned)sim.first<max_steps_in_simulation)
			cout<<".";
		else
			cout<<"x";
	}
	cout<<" ]";

	cout<<" Moves:[ ";
	for(auto sim:sim_results)
	{
			cout << setw(4) << std::fixed<< sim.first<<" ";
	}
	cout<<"]";

	cout<<" Rewards:[ ";
	for(auto sim:sim_results)
	{
			cout<< setw(4) << setprecision(2) << sim.second<<" ";
			rewards.push_back(sim.second);
	}
	cout<<"]";

	// pair: <numberof_bellmanupdate, rewards_for_that_bellmanupdate>
	// To plot bellman vs reward graphic.
	auto pair = make_pair(numberof_bellmanupdate, rewards );
	rewards_cumulative.push_back(pair);

	//cout<<endl<<endl;
}

pair<unsigned,double> Agent::Simulate(void)
{
	// Hold the reward that we will return.
	double reward=0;

	// Count the number of total movement.
	unsigned number_of_movement = 0;

	// Current State of agent
	SmartVector state = environment->Get_Random_State();

	cout<<" Init: [";
	for (int i = 0; i < state.dimension; ++i) {
		cout<<setw(2)<<(int)state.elements[i]<<" ";
	}
	cout<<"] ";

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

	return make_pair(number_of_movement,reward);
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

float Agent::Get_Score(void)
{
	auto pair = rewards_cumulative[rewards_cumulative.size()-1];
	float sum = 0;

	for (unsigned j = 0; j < pair.second.size(); ++j) {
		sum += pair.second[j];
	}
	return sum/pair.second.size();
}
