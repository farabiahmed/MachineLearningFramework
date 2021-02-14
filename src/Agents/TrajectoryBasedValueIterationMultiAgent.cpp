/*
 * TrajectoryBasedValueIteration.cpp
 *
 *  Created on: Mar 22, 2017
 *      Author: farabiahmed
 */

#include "Agents/TrajectoryBasedValueIterationMultiAgent.hpp"

bool TrajectoryBasedValueIterationMultiAgent::Start_Execution()
{
	unsigned numberof_processedtrajectorysteps = 0;
	bool epsilonProbabilityAutoMode = true;

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
		else if (!userCommand.substr(0,19).compare("EPSILON_PROBABILITY"))
		{
			 // Example:
			 // EPSILON_PROBABILITY=0.2 	to set a specific value
			 // EPSILON_PROBABILITY 		to set auto mode again
			 unsigned pos = userCommand.find("=");
			 if(pos != string::npos)
			 {
				 epsilonProbability = Convert::string_to_T<double>(userCommand.substr(pos + 1));
			 	 cout << "New epsilon set probability: " << epsilonProbability << endl;
			 	 epsilonProbabilityAutoMode = false;
			 }
			 else
			 {
				 epsilonProbabilityAutoMode = true;
			 }
		}
		else if(userCommand=="qvalue")
		{
			//Show Q-Values
			valueFunction->Print_Value();
		}

		cout<<"Game #: " << setw(6) << num_of_iteration << flush;

		// Q Value Update Value (currentQ-expectedQ)
		vector<double> diff;

		// Keep track of each trajectory element number for troubleshooting
		numberof_processedtrajectorysteps=0;

		// Create trajectory instance
		vector<pair<SmartVector,SmartVector>> trajectory;

		SmartVector state = environment->Get_Random_State();
		SmartVector action;

		// Determine state and action trajectory
		//while (isTrajectoryHaveTerminalState==false)
		//{
			//if(trajectory.size()>0) cout<<" Bad Trajectory ";
			//trajectory.clear();
			for (unsigned int i = 0; i < length_of_trajectory; ++i) {

				// Get next action to apply (Exploitation-Exploration)
				action  = Epsilon_Greedy_Policy(state);

				// Combine current state and action
				trajectory.push_back(make_pair(state,action));

				// Get next state by iterating previous state and action.
				state = environment->Get_Next_State(state,action);

				if(environment->Check_Terminal_State(state))
				{
					break;
				}
			}
		//}
		// For troubleshooting purposes
		// Show_Trajectory(trajectory);

		cout<<" Traject:"<<setw(4) << trajectory.size() << flush;
		cout<<" E-Prob:"<< setw(4) << setprecision(2) << epsilonProbability << flush;
		cout<<" Belman:" << flush;
		for (unsigned int i = 0; i < trajectory.size(); ++i)
		{
			// Get current pair
			state = trajectory[i].first;
			action  = trajectory[i].second;

			if(environment->Check_Terminal_State(state)){
				cout<<" Terminal State!";
			}
			else if (environment->Check_Blocked_State(state)){
				cout<<" Blocked State!";
			}
			else
			{
				double Q_plus = 0.0;
				vector<double> Q_pluses;

				// Load prospective nextstates with experiments.
				for (unsigned int i = 0; i < sample_length_L1; ++i) {

					// Get next state from environment model
					SmartVector nextState = environment->Get_Next_State(state,action);

					// Get Reward for tuple (state,action,state')
					double reward = environment->Get_Reward(state,action,nextState);
					vector<double> rewards = environment->Get_Rewards();

					// Get max Q value for next state.
					//double maxQvalue 	= Get_Greedy_Value(nextState,environment->Get_Action_List(state));
					//double maxQvalue 	= valueFunction->Get_Greedy_Pair(nextState).second;
					vector<double> maxQvalues 	= valueFunction->Get_Greedy_Pairs(nextState).second;

					// Update Q_plus
					//Q_plus += (1.0/(double)sample_length_L1) * ( reward + gamma * maxQvalue ) ;
					Q_pluses = vector<double>(maxQvalues.size());
					for(size_t j=0; j<maxQvalues.size(); j++)
					{
						Q_pluses[j] += (1.0/(double)sample_length_L1) * ( rewards[j] + gamma * maxQvalues[j] ) ;
					}

					//if(reward>0.8)
					//	cout<<endl<<"Reward:"<<reward<<" maxQ:"<<maxQvalue<<" Q_plus:"<<Q_plus<<endl;

					//cout<< numberof_bellmanupdate <<"\r" << flush;
					numberof_bellmanupdate++;
				}

				double currentQValue = 0;//valueFunction->Get_Value(state,action);

				// Update the difference value between new and old value
				diff.push_back(abs( currentQValue - Q_plus));

				// Count the valid trajectory steps for troubleshooting purposes
				numberof_processedtrajectorysteps++;

				// Update Value
				//valueFunction->Set_Value(state,action,Q_plus);
				valueFunction->Set_Values(state,action,Q_pluses);

				/*
				if(numberof_bellmanupdate % bellman_stride_forsimulation == 0)
				{
					// Get the cumulative rewards for the current bellman update.
					Get_Cumulative_Rewards(numberof_bellmanupdate);
				}
				*/
			}
		}// Trajectory Loop

		//cout<< " "<<flush;
		//cout<<" Belman:"<<numberof_bellmanupdate << flush;
		cout<< setw(8) << numberof_bellmanupdate<<flush;

		// Calculate mean diff
		double sum_diff = 0;
		double mean_diff = 0;
		for (unsigned i = 0; i < diff.size(); ++i) {
			sum_diff += diff[i];
		}
		mean_diff = sum_diff / (double) (diff.size());
		cout<<" Diff: "<< setw(10) << setprecision(6) << mean_diff;


		logger_agentStats<<num_of_iteration<<",";
		logger_agentStats<<trajectory.size()<<",";
		logger_agentStats<<epsilonProbability<<",";
		logger_agentStats<<numberof_bellmanupdate<<",";
		logger_agentStats<<mean_diff;
		logger_agentStats<<endl;

		/*
		// Check whether stopping criteria reached.
		if(mean_diff<epsilon)
		{
			cout << "Stopping Iterations. Diff: " << mean_diff <<endl;

			// If agent finishes before reaching BELLMAN_STRIDE_FORSIMULATION
			// run a simulation to evaluate performance.
			//if(!rewards_cumulative.size())
			//	Get_Cumulative_Rewards(numberof_bellmanupdate);

			// Run last simulation
			Get_Cumulative_Rewards(numberof_bellmanupdate);

			return true;
		}
		*/

		if(epsilonProbabilityAutoMode == true && epsilonProbability>=epsilonProbabilityEndValue)
		{
			if(epsilonProbabilityDecayRate<1.0)
				epsilonProbability *= epsilonProbabilityDecayRate;
			else
				epsilonProbability = (epsilonProbabilityStartValue - ((double)num_of_iteration / (double)max_number_of_iterations)*(epsilonProbabilityStartValue- epsilonProbabilityEndValue));
		}
	}// End of iterations loop
	return false;
}

