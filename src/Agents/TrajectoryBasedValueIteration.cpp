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
	epsilonProbabilityEndValue = cfg.GetValueOfKey<double>("EPSILON_PROBABILITY",0.2);

	epsilonProbabilityStartValue = cfg.GetValueOfKey<double>("EPSILON_PROBABILITY_START",0.6);

	// Epsilon Probability Decay Rate
	epsilonProbabilityDecayRate = cfg.GetValueOfKey<double>("EPSILON_PROBABILITY_DECAYRATE",1.1);

	epsilonProbability = epsilonProbabilityStartValue;

	// Number of next state samples
	// It will be used to approximate the expectations on the next state.
	sample_length_L1 = cfg.GetValueOfKey<int>("SAMPLE_LENGTH_L1",100);

    double probability_of_random_action = cfg.GetValueOfKey<double>("PROBABILITY_OF_RANDOM_ACTION",0.20);
	if( probability_of_random_action < 0.01 && sample_length_L1>1)
	{
		cout<<" L1 length is higher than required. Setting to 1" << flush;
		sample_length_L1 = 1;
	}

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

	// Get the last line of report if exist
	logger_agentStats.seekg(0, std::ios_base::beg);
	string lastline,temp;
	while( getline(logger_agentStats, temp)){if(temp.size()>1)lastline = temp;}
	logger_agentStats.clear(); //cleanup error and eof flags

	if(!lastline.size())
	{
		// Put Header first
		logger_agentStats<<"game,moves,egreedy,bellman,diff"<<endl<<flush;
		num_of_iteration = 0;
		numberof_bellmanupdate=0;
	}
	else
	{
		cout << "Continuing to the game found: " << lastline << endl;
		//There are some leftovers, sync!
		vector<string> info = Convert::Parse_String(lastline,',');
		num_of_iteration = (int) stoi(info[0]) + 1;
		numberof_bellmanupdate = (int) stoi(info[3]);

		//epsilonProbability = (double) stod(info[2]);
		epsilonProbability = (epsilonProbabilityStartValue - ((double)num_of_iteration / (double)max_number_of_iterations)*(epsilonProbabilityStartValue- epsilonProbabilityEndValue));
	}
}

TrajectoryBasedValueIteration::~TrajectoryBasedValueIteration()
{
	logger_agentStats.close();
}

bool TrajectoryBasedValueIteration::Start_Execution()
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

					//cout<< numberof_bellmanupdate <<"\r" << flush;
					numberof_bellmanupdate++;
				}

				double currentQValue = valueFunction->Get_Value(state,action);

				// Update the difference value between new and old value
				diff.push_back(abs( currentQValue - Q_plus));

				// Count the valid trajectory steps for troubleshooting purposes
				numberof_processedtrajectorysteps++;

				// Update Value
				valueFunction->Set_Value(state,action,Q_plus);

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
