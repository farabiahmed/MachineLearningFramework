/*
 * QIteration.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: farabiahmed
 */

#include <Agents/QIteration.hpp>

QIteration::QIteration(const Environment* env, const Representation* rep, const ConfigParser& cfg)
: Agent(cfg)
{
	Name = "Q-Iteration Agent";

	// Dependency Injection Pattern, constructor style
	environment = (Environment*)env;

	// Dependency Injection Pattern, constructor style
	valueFunction = (Representation*)rep;

	// Get the epsilon value.
	// If the total update is less than epsilon, we will terminate that iteration.
	epsilon = cfg.GetValueOfKey<double>("EPSILON",1e-5f);

	// Gamma - Discount Factor
	// This parameter balances current and future rewards.
	gamma = cfg.GetValueOfKey<double>("GAMMA",0.8);

	// Max Iteration
	// Total number of iteration to find the optimal policy.
	max_number_of_iterations = cfg.GetValueOfKey<double>("MAX_NUMBER_OF_ITERATION",50);

	// Number of simulations per iteration
	// It helps us to evaluate the performance of the agent by doing simulations in series.
	number_of_simulations = cfg.GetValueOfKey<unsigned>("NUMBER_OF_SIMULATIONS",10);

	// It prevents the agent to stuck in a loop in the environment
	// It is recommended that to keep it high to know whether it
	// is stucked in the cumulative rewards plot.
	max_steps_in_simulation = cfg.GetValueOfKey<unsigned>("MAX_STEPS_IN_SIMULATION",100);

	// Defines how many bellman updates should be discarded to make  a series of
	// simulations to show the performance of agent.
	bellman_stride_forsimulation = cfg.GetValueOfKey<unsigned>("BELLMAN_STRIDE_FORSIMULATION",50);

	states = environment->Get_All_Possible_States();
}

QIteration::~QIteration() {
	// TODO Auto-generated destructor stub
}

bool QIteration::Start_Execution()
{
	unsigned numberof_bellmanupdate = 0;

	for (int num_of_iteration = 0; num_of_iteration < max_number_of_iterations; num_of_iteration++)
	{
		// Test controller performance.
		Get_Cumulative_Rewards(numberof_bellmanupdate);
		cout << endl;

		if(userControl.GetMessage()=="stop")
		{
			break;
		}

		cout<<"Iteration #: " << num_of_iteration;

		// Q Value Update Value (currentQ-expectedQ)
		// initialize with zero
		vector<double> diff;

		// Loop through all states
		for(unsigned int index_state=0; index_state < states.size(); index_state++)
		{
			// Get Current State
			SmartVector currentState = states[index_state];

			// Acquire all available actions
			vector<SmartVector> actions = environment->Get_Action_List(currentState);

			// Loop through actions
			for(unsigned int index_action=0; index_action < actions.size(); index_action++)
			{
				SmartVector currentAction = actions[index_action];

				if(environment->Check_Terminal_State(currentState)){}
				else if(environment->Check_Blocked_State(currentState)){}
				else
				{
					// Initialize expected value of stochastic process
					double expectedValue=0;

     				// Get transition pair list (nextstate-probability) for state-action pair.
					vector<pair<SmartVector,double>> transitionPair	= environment->Get_Transition_Probability(currentState,currentAction);

					for (unsigned int i = 0; i < transitionPair.size(); ++i) {

						// Get possible next state
						SmartVector nextState = transitionPair[i].first;

						// Get max Q value for next state. <maxArg,maxVal>
						double maxQvalue 	= valueFunction->Get_Greedy_Pair(nextState).second;

						// Get Reward for tuple (state,action,state')
						double reward 		= environment->Get_Reward(currentState,currentAction,nextState);

						// Get probability
						double probability = transitionPair[i].second;

						// Run Bellman Equation
						expectedValue += probability * (reward + gamma * maxQvalue);
					}

					// Get the current Q value from representation object.
					double currentQValue = valueFunction->Get_Value(currentState,currentAction);

					// Update the difference value between new and old value
					diff.push_back(abs( currentQValue - expectedValue));

					// Update Q Value by newest estimate.
					valueFunction->Set_Value(currentState, currentAction, expectedValue);

					numberof_bellmanupdate++;

					//if((numberof_bellmanupdate % bellman_stride_forsimulation)==0)
						// Get the cumulative rewards for the current bellman update.
						//Get_Cumulative_Rewards(numberof_bellmanupdate);

				}
			}// End of action loop
		}// End of state loop

		// Calculate mean diff
		double sum_diff = 0;
		double mean_diff = 0;
		for (unsigned i = 0; i < diff.size(); ++i) {
			sum_diff += diff[i];
		}
		mean_diff = sum_diff / (double) diff.size();
		cout<<" Diff: "<< mean_diff <<endl;

		// Check whether stopping criteria reached.
		if(mean_diff<epsilon)
		{
			cout << "Stopping Iterations. Diff: " << mean_diff <<endl;

			// If agent finishes before reaching BELLMAN_STRIDE_FORSIMULATION
			// run a simulation to evaluate performance.
			if(!rewards_cumulative.size())
				Get_Cumulative_Rewards(numberof_bellmanupdate);

			return true;
		}

	}// End of iterations loop

	return false;
}
