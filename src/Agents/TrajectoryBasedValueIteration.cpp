/*
 * TrajectoryBasedValueIteration.cpp
 *
 *  Created on: Mar 22, 2017
 *      Author: farabiahmed
 */

#include "Agents/TrajectoryBasedValueIteration.hpp"

TrajectoryBasedValueIteration::TrajectoryBasedValueIteration(const Environment* env, const Representation* rep, const ConfigParser& cfg) {

	environment = (Environment*)env;

	valueFunction = (Representation*)rep;

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

	// Number of next state samples
	// It will be used to approximate the expectations on the next state.
	sample_length_L1 = cfg.GetValueOfKey<int>("SAMPLE_LENGTH_L1",100);

	// Trajectory Length
	// It will be used to determine the trajectory that the algorithm will follow for each iteration.
	length_of_trajectory = cfg.GetValueOfKey<int>("LENGTH_OF_TRAJECTORY",100);
}

TrajectoryBasedValueIteration::~TrajectoryBasedValueIteration() {

}

bool TrajectoryBasedValueIteration::Start_Execution()
{

	for (int num_of_iteration = 0; num_of_iteration < max_number_of_iterations; num_of_iteration++)
	{
		cout<<"Iteration #: " << num_of_iteration << endl;

		vector<pair<SmartVector,SmartVector>> trajectory;
		SmartVector state = environment->Get_Initial_State();
		SmartVector action;

		// Determine state and action trajectory
		for (unsigned int i = 0; i < length_of_trajectory; ++i) {

			// Get next action to apply (Exploitation-Exploration)
			action  = Epsilon_Greedy_Policy(state);

			// Combine current state and action
			trajectory.push_back(make_pair(state,action));

			// Get next state by iterating previous state and action.
			state = environment->Get_Next_State(state,action);

		}

		for (unsigned int i = 0; i < trajectory.size(); ++i) {

			// Create an instance for next states of
			// current trajectory[i] = <state,action> pair.
			//vector<SmartVector> nextStates;

			// Get current pair
			state = trajectory[i].first;
			action  = trajectory[i].second;

			// Print Out Visited States
			//cout<<state.index<<"-"<<action.index<<endl;

			//Test();

			if(environment->Check_Terminal_State(state)){}
			else if (environment->Check_Blocked_State(state)){}
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
				}

				// Update Value
				valueFunction->Set_Value(state,action,Q_plus);
			}
		}// Trajectory Loop
	}// End of iterations loop

	return false;
}


SmartVector TrajectoryBasedValueIteration::Epsilon_Greedy_Policy(const SmartVector& state) const
{
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
		//cout<<"Exploitation"<<endl;

		int argMax = valueFunction->Get_Greedy_Pair(state).first;
		//policy.push_back(actions[argMax]);

		action = actions[argMax];
	}

	// Release Memory
	delete p;

	return action;
	//return policy;
}

void TrajectoryBasedValueIteration::Test(void)
{
	cout<<"THIS IS TEST FUNCTION CALL:"<<endl;


	/*
	SmartVector action;
	SmartVector state = environment->Get_Initial_State();

	for (int i = 0; i < 100; ++i) {
		action = Epsilon_Greedy_Policy(state);
		action.Print();
	}
	*/

	/*

	vector<SmartVector> states = environment->Get_All_Possible_States();
	vector<SmartVector> actions = environment->Get_Action_List(states[0]);
	double QValue=0;
	int Policy[3][4];
	double maxQValue=0;

	for (unsigned int i = 0; i < states.size(); ++i) {

		maxQValue = -99;
		for (unsigned int j = 0; j < actions.size(); ++j) {

			QValue = valueFunction->Get_Value(states[i],actions[j]);

			if(QValue>maxQValue)
			{
				maxQValue = QValue;
				Policy[i/4][i%4] = j;
			}
			cout<< setw(10) << setprecision(5) << QValue << "     ";
		}
		cout<<endl;

	}


	for (int r = 0; r < 3; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			char ch = (Policy[r][c]==0) ? '^' : (Policy[r][c]==1) ? '>' : (Policy[r][c]==2) ? '_' : '<';

			if(r==1 && c==1) ch = 'x';
			if(r==0 && c==3) ch = '+';
			if(r==1 && c==3) ch = '-';

			cout<<ch;
			cout << "    ";
		}
		cout << " " << endl;
	}

	//theta.Print();
	 *
	 *
	 */

}
