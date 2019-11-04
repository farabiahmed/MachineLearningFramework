/*
 * TrajectoryBasedValueIteration.cpp
 *
 *  Created on: Mar 22, 2017
 *      Author: farabiahmed
 */

#include "Agents/RandomAgent.hpp"

RandomAgent::RandomAgent(const Environment* env, const ConfigParser& cfg)
: Agent(cfg)
{

	environment = (Environment*)env;

	valueFunction = NULL;

	Name = "Random Agent";

	// Max Iteration
	// Total number of iteration to find the optimal policy.
	max_number_of_iterations = cfg.GetValueOfKey<int>("MAX_NUMBER_OF_ITERATION",50);

	// Number of next state samples
	// It will be used to approximate the expectations on the next state.
	sample_length_L1 = cfg.GetValueOfKey<int>("SAMPLE_LENGTH_L1",100);

	// Trajectory Length
	// It will be used to determine the trajectory that the algorithm will follow for each iteration.
	length_of_trajectory = cfg.GetValueOfKey<int>("LENGTH_OF_TRAJECTORY",100);

	// Number of simulations per iteration
	// It helps us to evaluate the performance of the agent by doing simulations in series.
	number_of_simulations = cfg.GetValueOfKey<unsigned>("NUMBER_OF_SIMULATIONS",10);

	// It prevents the agent to stuck in a loop in the environment
	// It is recommended that to keep it high to know whether it
	// is stucked in the cumulative rewards plot.
	max_steps_in_simulation = cfg.GetValueOfKey<unsigned>("MAX_STEPS_IN_SIMULATION",100);

	num_of_iteration=0;
}

RandomAgent::~RandomAgent()
{

}

bool RandomAgent::Start_Execution()
{
	double epsilonProbability = 1.0;

	// Test initial controller performance.
	//Get_Cumulative_Rewards(numberof_bellmanupdate);

	for (; num_of_iteration < max_number_of_iterations; num_of_iteration++)
	{
		// Test controller performance.
		Get_Cumulative_Rewards(numberof_bellmanupdate);
		cout << endl;

		string userCommand = userControl.GetMessage();
		if(userCommand=="stop")
		{
			break;
		}

		cout<<"Game #: " << setw(6) << num_of_iteration << flush;

		logger_agentStats<<num_of_iteration<<",";
		logger_agentStats<<0<<",";
		logger_agentStats<<epsilonProbability<<",";
		logger_agentStats<<numberof_bellmanupdate<<",";
		logger_agentStats<<0;
		logger_agentStats<<endl;

		numberof_bellmanupdate+=1;

	}// End of iterations loop
	return false;
}

