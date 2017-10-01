/*
 * GreedyAgent.cpp
 *
 *  Created on: Sep 21, 2017
 *      Author: farabiahmed
 */

#include <Agents/GreedyAgent.hpp>

GreedyAgent::GreedyAgent(const Environment* env, const ConfigParser& cfg) : Agent(cfg)
{
	environment = (Environment*)env;

	max_steps_in_simulation = cfg.GetValueOfKey<unsigned>("MAX_STEPS_IN_SIMULATION",100);
}

bool GreedyAgent::Start_Execution()
{
	vector<SmartVector> actions_selected;
	vector<double> rewards;
	size_t step_counter=0;

	SmartVector state = environment->Get_Initial_State();
	vector<SmartVector> actions = environment->Get_Action_List(state);

	double reward=0;

	// Move the agents until they reach terminal state.
	while(step_counter <= max_steps_in_simulation)
	{
		cout<<endl;
		cout<<"Displaying Step #"<<step_counter<<endl;
		environment->Display_State(state);


		double maxValue = std::numeric_limits<double>::lowest();

		SmartVector actionGreedy;

		//Find Greedy Action
		for(auto action:actions)
		{
			SmartVector nextState = environment->Get_Next_State(state,action);
			reward = environment->Get_Reward(state,action,nextState);

			if(reward > maxValue)
			{
				maxValue = reward;
				actionGreedy = action;
			}

		}
		//cout<<"Reward:"<<reward<<endl;

		actions_selected.push_back(actionGreedy);

		rewards.push_back(reward);

		step_counter++;

		if(environment->Check_Terminal_State(state)) break;

		// Move the agents to new state with the greedy action.
		state = environment->Get_Next_State(state,actionGreedy);
	}

	PrintActions(actions_selected);
	PrintRewards(rewards);


	return true;
}

void GreedyAgent::PrintActions(const vector<SmartVector>& actions)
{
	int numberOfAgents = actions[0].size();

	cout<<endl<<"List of actions that applied:"<<endl;

	for (int i = 0; i < numberOfAgents; ++i) {

		cout<<"Actions for Agent #"<<i<<" : ";

		for (unsigned j = 0; j < actions.size(); ++j) {

			int actId = (int)actions[j].elements[i];

			if(actId==0) cout<<"\u2191 ";
			else if(actId==1) cout<<"\u2192 ";
			else if(actId==2) cout<<"\u2193 ";
			else if(actId==3) cout<<"\u2190 ";
			else if(actId==4) cout<<"o ";
		}

		cout<<endl;
	}
}

void GreedyAgent::PrintRewards(const vector<double>& rewards)
{
	double cumulativeReward=0;

	cout<<endl<<"Collected rewards : "<<endl;

	for (unsigned i = 0; i < rewards.size(); ++i)
	{
		cout<<rewards[i]<<" ";
		cumulativeReward+=rewards[i];
	}
	cout<<endl;

	cout<<endl<<"Cumulative Reward:"<<endl;
	cout<<cumulativeReward<<endl;

	cout<<endl;
}
GreedyAgent::~GreedyAgent() {
	// TODO Auto-generated destructor stub
}

