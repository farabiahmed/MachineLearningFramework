/*
 * QIteration.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: farabiahmed
 */

#include <Agents/QIteration.hpp>

QIteration::QIteration(const Environment* env, const Representation* rep, const ConfigParser& cfg) {

	// Dependency Injection Pattern, constructor style
	environment = (Environment*)env;

	// Dependency Injection Pattern, constructor style
	valueFunction = (Representation*)rep;

	// Inform the user about environment name.
	cout << "Environment Name: " << environment->Name << endl;

	// Get the epsilon value.
	// If the total update is less than epsilon, we will terminate that iteration.
	epsilon = cfg.GetValueOfKey<double>("EPSILON",1e-5f);

	// Gamma - Discount Factor
	// This parameter balances current and future rewards.
	gamma = cfg.GetValueOfKey<double>("GAMMA",0.8);

	// Max Iteration
	// Total number of iteration to find the optimal policy.
	max_number_of_iterations = cfg.GetValueOfKey<double>("MAX_NUMBER_OF_ITERATION",50);

}

QIteration::~QIteration() {
	// TODO Auto-generated destructor stub
}

bool QIteration::Start_Execution()
{
	for (int num_of_iteration = 0; num_of_iteration < max_number_of_iterations; num_of_iteration++)
	{
		cout<<"Iteration #: " << num_of_iteration << endl;

		double diff=0;

		vector<SmartVector> states = environment->Get_All_Possible_States();

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

					double currentQValue = valueFunction->Get_Value(currentState,currentAction);

					// Update the difference value between new and old value if the current one is bigger than old one.
					if( abs( currentQValue - expectedValue ) > diff)
					{
						diff = abs( currentQValue - expectedValue);
					}

					// Update Q Value by newest estimate.
					valueFunction->Set_Value(currentState, currentAction, expectedValue);
				}
			}// End of action loop
		}// End of state loop

		// Check whether stopping criteria reached.
		if(diff<epsilon)
		{
			cout << "Stopping Iterations. Diff: " << diff <<endl;
			return true;
		}

	}// End of iterations loop

	return false;
}

void QIteration::Test(void)
{

}
