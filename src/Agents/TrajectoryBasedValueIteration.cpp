/*
 * TrajectoryBasedValueIteration.cpp
 *
 *  Created on: Mar 22, 2017
 *      Author: farabiahmed
 */

#include "Agents/TrajectoryBasedValueIteration.hpp"

TrajectoryBasedValueIteration::TrajectoryBasedValueIteration(const Environment* env, const Representation* rep, const ConfigParser& cfg)
: Agent(cfg)
{

	environment = (Environment*)env;

	valueFunction = (Representation*)rep;

	Name = "Trajectory Based Value Iteration Agent";

	// Get the epsilon value.
	// If the total update is less than epsilon, we will terminate that iteration.
	epsilon = cfg.GetValueOfKey<double>("EPSILON",1e-5f);

	// Gamma - Discount Factor
	// This parameter balances current and future rewards.
	gamma = cfg.GetValueOfKey<double>("GAMMA",0.8);

	// Max Iteration
	// Total number of iteration to find the optimal policy.
	max_number_of_iterations = cfg.GetValueOfKey<int>("MAX_NUMBER_OF_ITERATION",50);

	// Explore-Exploit Parameter.
	// Probability that balances exploration and exploitation
	epsilonProbability = cfg.GetValueOfKey<double>("EPSILON_PROBABILITY",0.8);

	// Epsilon Probability Decay Rate
	epsilonProbabilityDecayRate = cfg.GetValueOfKey<double>("EPSILON_PROBABILITY_DECAYRATE",1.0);

	if(epsilonProbabilityDecayRate != 1.0)
		epsilonProbability = 1.0;

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

	// Defines how many bellman updates should be discarded to make simulation to
	// plot the performance of agent.
	bellman_stride_forsimulation = cfg.GetValueOfKey<unsigned>("BELLMAN_STRIDE_FORSIMULATION",50);


	states = environment->Get_All_Possible_States();

	// Put Header first
	logger<<"game,moves,egreedy,bellman"<<endl<<flush;
}

TrajectoryBasedValueIteration::~TrajectoryBasedValueIteration()
{
	logger.close();
}

bool TrajectoryBasedValueIteration::Start_Execution()
{
	unsigned numberof_bellmanupdate = 237000;
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

		cout<<"Iteration #: " << num_of_iteration << flush;

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

		cout<<" Traject:"<<trajectory.size() << flush;
		cout<<" E-Prob:"<<epsilonProbability << flush;
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

				// Count the valid trajectory steps for troubleshooting purposes
				numberof_processedtrajectorysteps++;

				// Update Value
				valueFunction->Set_Value(state,action,Q_plus);

				//cout<< numberof_bellmanupdate <<"\r" << flush;
				numberof_bellmanupdate++;

				if(numberof_bellmanupdate % bellman_stride_forsimulation == 0)
				{
					// Get the cumulative rewards for the current bellman update.
					Get_Cumulative_Rewards(numberof_bellmanupdate);
				}
			}
		}// Trajectory Loop

		//cout<< " "<<flush;
		//cout<<" Belman:"<<numberof_bellmanupdate << flush;
		cout<<numberof_bellmanupdate<<flush;

		logger<<num_of_iteration<<",";
		logger<<trajectory.size()<<",";
		logger<<epsilonProbability<<",";
		logger<<numberof_bellmanupdate;
		logger<<endl;

		// Calculate mean diff
		double sum_diff = 0;
		double mean_diff = 0;
		for (unsigned i = 0; i < diff.size(); ++i) {
			sum_diff += diff[i];
		}
		mean_diff = sum_diff / (double) (diff.size());
		cout<<" Diff: "<< mean_diff <<endl;

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
		if(epsilonProbability>0.1)
			epsilonProbability *= epsilonProbabilityDecayRate;

	}// End of iterations loop
	return false;
}


SmartVector TrajectoryBasedValueIteration::Epsilon_Greedy_Policy(const SmartVector& state) const
{
	// Measure propability engine performance
	// by keeping track of those variables.
	static unsigned int exploration=0;
	static unsigned int exploitation=0;

	// Create return parameter.
	//vector<SmartVector> policy;
	SmartVector action;

	// Store all available actions to use in exploration.
	vector<SmartVector> actions = environment->Get_Action_List(state);

	// Create an instance for discrete probabilities
	vector<double> probabilities;

	// Create experiment engine
	ProbabilityDistribution *p = nullptr;
	vector<int> outcomes;

	//for (unsigned int i = 0; i < sample; ++i) {

	// Re-assign probability engine.
	probabilities.clear();
	probabilities = {epsilonProbability, 1.0-epsilonProbability};
	delete p;
	p = new DiscreteDistribution(probabilities);

	// Run a random experiment
	outcomes.clear();
	outcomes = p->Run_Experiment(1);

	// Exploration
	// Select an action from list with uniformly distributed experiment.
	// with a small probability epsilon.
	if(outcomes[0])
	{
		exploration++;
		// Run new probability experiment with uniform parameters.
		//cout<<"Exploration"<<endl;

		// Get the number of candidate actions
		int count = actions.size();

		// Fill the distribution with equal uniform parameters.
		// Ex: If number of actions are 4: each probability will be 0.25.
		probabilities.clear();
		for (int i = 0; i < count; ++i) {
			probabilities.push_back(1.0/count);
		}

		// Run probability engine with different parameters
		delete p;
		p = new DiscreteDistribution(probabilities);
		outcomes.clear();
		outcomes = p->Run_Experiment(1);


		for (unsigned int i = 0; i < outcomes.size(); ++i) {
			if(outcomes[i])
			{
				//cout<<"\tAction: #" << i << endl;
				action = actions[i];
				break;
			}
		}

	}
	// Exploitation
	// Select an action greedily with respect to QValue.
	else
	{
		exploitation++;
		//cout<<"Exploitation"<<endl;

		int argMax = valueFunction->Get_Greedy_Pair(state).first;
		//policy.push_back(actions[argMax]);

		action = actions[argMax];
	}

	// Release Memory
	delete p;

	// Measure probability engine performance.
	//cout<<endl<<"Experienced Exploitation-Exploration Epsilon: " << (float)exploration/(float)(exploitation+exploration) <<endl;

	return action;
}

void TrajectoryBasedValueIteration::Show_Trajectory(vector<pair<SmartVector,SmartVector>> trajectory) const
{
	cout<<endl<<"---------------------------"<<endl;
	cout<<"Trajectory Details:"<<endl;

	for (unsigned int i = 0; i < trajectory.size(); ++i)
	{
		// Get current pair
		SmartVector state = trajectory[i].first;
		SmartVector action  = trajectory[i].second;

		cout<<"#"<<i<<" State: [ ";
		for (int j = 0; j < state.size(); ++j) {
			cout<<state.elements[j]<<" ";
		}
		cout<<"] ";

		cout<<"Action: [ ";
		for (int j = 0; j < action.size(); ++j) {
			cout<<action.elements[j]<<" ";
		}
		cout<<"]";


		if(environment->Check_Terminal_State(state)){
			cout<<" Terminal State";
		}
		else if (environment->Check_Blocked_State(state)){
			cout<<" Blocked State";
		}
		cout<<endl;
	}
	cout<<"---------------------------"<<endl;
}
