// A.cpp
#include "QIteration/CleaningRobotDeterministic.hpp"

CleaningRobotDeterministic::CleaningRobotDeterministic(void)
{
	cout<<"Class CleaningRobotDeterministic Constructor"<<endl;

	//Initialize Public Variables

	max_number_of_iterations = 5;
	gamma = 0.5;
	epsilon = 0.001;

	//Initialize Private Variables

	number_of_states = 6;
	number_of_actions = 2;

	states = new int[number_of_states] {0, 1, 2, 3, 4, 5};
	actions = new int[number_of_actions] {-1,1};

	for (int i = 0; i < number_of_states; ++i)
	{
		for (int j = 0; j < number_of_actions; ++j)
		{
			Q[i][j] = 0;
		}
	}
}

CleaningRobotDeterministic::~CleaningRobotDeterministic(void)
{
	cout<<"Class CleaningRobotDeterministic Destructor"<<endl;
}

bool CleaningRobotDeterministic::Run_Iteration()
{
	cout<<"CleaningRobotDeterministic Running Iteration"<<endl;

	for (int num_of_iteration = 0; num_of_iteration < max_number_of_iterations; num_of_iteration++)
	{
		cout<<"Iteration #: " << num_of_iteration << endl;

		for (int index_of_state = 0; index_of_state < number_of_states; index_of_state++)
		{
			for (int index_of_action = 0; index_of_action < number_of_actions; index_of_action++)
			{
				int s = states[index_of_state];
				int a = actions[index_of_action];
				int r = Reward(s,a);
				int m = Model(s,a);

				//cout << "s:" <<s << " a:"  << a << " r:" << r << " m:" << m << " Q:" << Q[m][index_of_action] << endl;
				//cout << "Test" <<  index_of_state << " " << index_of_action << endl;				
				//Q[index_of_state][index_of_action] = 5.41;

				//Q[index_of_state][index_of_action] = r + gamma * max_u ( Q[m][index_of_action] );
				Q[index_of_state][index_of_action] = r + gamma * ( ( Q[m][0] > Q[m][1] ) ? Q[m][0] : Q[m][1] );

			}
		}
		//Print_Q_Value();
	}


  	return false;
}

int CleaningRobotDeterministic::Model(int x, int u)
{
	if(x<=4 && x>=1)
		return x + u;
	else
		return x;
}

int CleaningRobotDeterministic::Reward(int x, int u)
{
	int r=0;

	if(x==4 && u==1)
		r = 5;
	else if(x==1 && u==-1)
		r = 1;
	else
		r = 0;

  	return r;
}

void CleaningRobotDeterministic::Print_Q_Value(void)
{
	cout << "Q Values:" << endl;
	for (int i = 0; i < number_of_states; ++i)
	{
		for (int j = 0; j < number_of_actions; ++j)
		{
			cout << Q[i][j] << "    ";
		}
		cout << "" << endl;
	}
}