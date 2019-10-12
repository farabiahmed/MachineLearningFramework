/*
 * DeepQNetwork.cpp
 *
 *  Created on: Oct 9, 2017
 *      Author: farabiahmed
 */

#include <Agents/DeepQNetwork.hpp>

DeepQNetwork::DeepQNetwork(const Environment* env, const Representation* rep, const ConfigParser& cfg) :
TrajectoryBasedValueIteration(env,rep,cfg)
{

}

DeepQNetwork::~DeepQNetwork() {

}


bool DeepQNetwork::Start_Execution()
{
	unsigned numberof_bellmanupdate = 0;
	unsigned numberof_processedtrajectorysteps = 0;

	// Test initial controller performance.
	Get_Cumulative_Rewards(numberof_bellmanupdate);

	for (int num_of_iteration = 0; num_of_iteration < max_number_of_iterations; num_of_iteration++)
	{
		string userCommand = userControl.GetMessage();
		if(userCommand=="stop")
		{
			break;
		}
		else if(userCommand=="qvalue")
		{
			//Show Q-Values
			valueFunction->Print_Value();
		}

		// Keep track of each trajectory element number for troubleshooting
		numberof_processedtrajectorysteps=0;

		// Keep experience elements.
		SmartVector state = environment->Get_Random_State();
		SmartVector action;
		SmartVector nextState;
		double reward = 0;
		int status = 0;


		for (unsigned int i = 0; i < length_of_trajectory; ++i) {

			// Count the valid trajectory steps for troubleshooting purposes
			numberof_processedtrajectorysteps++;

			// Get next action to apply (Exploitation-Exploration)
			action  = Epsilon_Greedy_Policy(state);

			// Get next state from environment model
			nextState = environment->Get_Next_State(state,action);

			// Get Reward for tuple (state,action,state')
			reward = environment->Get_Reward(state,action,nextState);

			// Gather some data to make statistics.
			numberof_bellmanupdate++;

			if(numberof_bellmanupdate % bellman_stride_forsimulation == 0)
			{
				// Get the cumulative rewards for the current bellman update.
				Get_Cumulative_Rewards(numberof_bellmanupdate);
			}

			if(environment->Check_Terminal_State(nextState))
			{
				// Set status to one to indicate this experience reaches terminal.
				status = 1;
				valueFunction->Add_Experience(state,action,nextState,reward,status);
				break;
			}
			else
			{
				// Set status to zero to indicate this experience does not reach terminal.
				status = 0;
				valueFunction->Add_Experience(state,action,nextState,reward,status);
			}

			// Move to nextState to get a new experience
			state = nextState;
		}

		if(epsilonProbability>0.1)
			epsilonProbability *= epsilonProbabilityDecayRate;

		cout<<"Game #: " << num_of_iteration << flush;
		cout<<" Traject:"<<numberof_processedtrajectorysteps << flush;
		cout<<" E-Prob:"<<epsilonProbability << flush;
		cout<<" Belman:" << flush;
		cout<<numberof_bellmanupdate<<flush;
		cout<<endl;

		logger_agentStats<<num_of_iteration<<",";
		logger_agentStats<<numberof_processedtrajectorysteps<<",";
		logger_agentStats<<epsilonProbability<<",";
		logger_agentStats<<numberof_bellmanupdate;
		logger_agentStats<<endl;

	}// End of iterations loop
	return false;
}
