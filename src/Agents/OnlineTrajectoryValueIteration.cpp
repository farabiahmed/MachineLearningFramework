/*
 * OnlineTrajectoryValueIteration.cpp
 *
 *  Created on: Sep 25, 2017
 *      Author: farabiahmed
 */

#include <Agents/OnlineTrajectoryValueIteration.hpp>

OnlineTrajectoryValueIteration::OnlineTrajectoryValueIteration(const Environment* env, const Representation* rep, const ConfigParser& cfg) :
TrajectoryBasedValueIteration(env,rep,cfg)
{
	// Log parameters
	// Create File Directory First
	system(("mkdir -p " + log_file_path).c_str());

	logger_agentStats.open((log_file_path + "/agentReport2.csv").c_str(),fstream::out);

	// Put Header first
	logger_agentStats<<"game,moves,egreedy,bellman"<<endl<<flush;

}

OnlineTrajectoryValueIteration::~OnlineTrajectoryValueIteration()
{
	logger_agentStats.close();
}


bool OnlineTrajectoryValueIteration::Start_Execution(void)
{
	unsigned numberof_bellmanupdate = 0;
	unsigned number_of_moves = 0;

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

		cout<<"Game #: " << num_of_iteration << flush;

		// Q Value Update Value (currentQ-expectedQ)
		vector<double> diff;

		// Keep track of each trajectory element number for troubleshooting
		number_of_moves=0;

		// Create trajectory instance
		vector<pair<SmartVector,SmartVector>> trajectory;
		bool isTrajectoryHaveTerminalState=false;

		SmartVector state = environment->Get_Random_State();
		SmartVector action = Epsilon_Greedy_Policy(state);

		while (isTrajectoryHaveTerminalState==false)
		{
			if(environment->Check_Terminal_State(state)){
				cout<<" Terminal State!";
				isTrajectoryHaveTerminalState=true;
			}
			else if (environment->Check_Blocked_State(state)){
				cout<<" Blocked State!";
			}
			else
			{
				double Q_plus = 0.0;

				// Load prospective nextstates with experiments.
				for (unsigned int i = 0; i < sample_length_L1; ++i) {

					// Get next state from environment model
					SmartVector nextState = environment->Get_Next_State(state,action);

					// Get Reward for tuple (state,action,state')
					double reward = environment->Get_Reward(state,action,nextState);

					// Get max Q value for next state.
					//double maxQvalue 	= Get_Greedy_Value(nextState,environment->Get_Action_List(state));
					double maxQvalue 	= valueFunction->Get_Greedy_Pair(nextState).second;

					// Update Q_plus
					Q_plus += (1.0/(double)sample_length_L1) * ( reward + gamma * maxQvalue ) ;

					//if(reward>0.8)
					//	cout<<endl<<"Reward:"<<reward<<" maxQ:"<<maxQvalue<<" Q_plus:"<<Q_plus<<endl;

				}

				double currentQValue = valueFunction->Get_Value(state,action);

				// Update the difference value between new and old value
				diff.push_back(abs( currentQValue - Q_plus));

				// Count the valid moves for troubleshooting purposes
				number_of_moves++;

				// Update Value
				valueFunction->Set_Value(state,action,Q_plus);

				//cout<< numberof_bellmanupdate <<"\r" << flush;
				numberof_bellmanupdate++;

				if(numberof_bellmanupdate % bellman_stride_forsimulation == 0)
				{
					// Get the cumulative rewards for the current bellman update.
					Get_Cumulative_Rewards(numberof_bellmanupdate);
				}

				// Get next state by iterating previous state and action.
				state = environment->Get_Next_State(state,action);

				// Get next action to apply (Exploitation-Exploration)
				action  = Epsilon_Greedy_Policy(state);
			}
		}// Trajectory Loop

		cout<<" Moves:" << number_of_moves << flush;
		cout<<" E-Prob:"<< epsilonProbability << flush;
		cout<<" Belman:" << flush;
		cout<< numberof_bellmanupdate << flush;

		logger_agentStats<<num_of_iteration<<",";
		logger_agentStats<<number_of_moves<<",";
		logger_agentStats<<epsilonProbability<<",";
		logger_agentStats<<numberof_bellmanupdate;
		logger_agentStats<<endl;

		// Calculate mean diff
		double sum_diff = 0;
		double mean_diff = 0;
		for (unsigned i = 0; i < diff.size(); ++i) {
			sum_diff += diff[i];
		}
		mean_diff = sum_diff / (double) (diff.size());
		cout<<" Diff: "<< mean_diff <<endl;


		if(epsilonProbability>0.1)
			epsilonProbability *= epsilonProbabilityDecayRate;

	}// End of iterations loop
	return false;
}
