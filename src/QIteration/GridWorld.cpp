// Gridworld.cpp

#include <cmath>
#include <iomanip>
#include "QIteration/GridWorld.hpp"

GridWorld::GridWorld(void):
number_of_states_row(3), number_of_states_col(4), number_of_actions(4),
max_number_of_iterations(200), gamma(1), epsilon(1e-10f),
probability_for_desired_action(0.8f), probability_for_left_action (0.1f), probability_for_right_action (0.1f)
{
	cout<<"Class GridWorld Constructor"<<endl;

	for (int i = 0; i < number_of_states_row; i++)
	{
		for (int j = 0; j < number_of_states_col; j++)
		{
			Policy[i][j] = -1; //Set to undefined value. defined values: 0,1,2,3 (N,E,S,W)

			for (int k = 0; k < number_of_actions; k++)
			{
				Q[i][j][k] = 0;
			}
		}
	}
}

GridWorld::~GridWorld(void)
{
	cout<<"Class GridWorld Destructor"<<endl;
}

bool GridWorld::Run_Iteration()
{
	cout<<"GridWorld Running Iteration"<<endl;

	for (int num_of_iteration = 0; num_of_iteration < max_number_of_iterations; num_of_iteration++)
	{
		cout<<"Iteration #: " << num_of_iteration << endl;

		double diff=0;

		// iterate for every (x,u)
		// x has 2 dimension (row, column)
		// state x represented by (x_row, x_col)		
		// u has 1 dimension (direction: north, east, south,west)


		for (int x_row = 0; x_row < number_of_states_row; x_row++)
		{
			for (int x_col = 0; x_col < number_of_states_col; x_col++)
			{

				for (int action = 0; action < number_of_actions; action++)
				{

					if (x_row==0 && x_col==3)
					{
						Q[x_row][x_col][action] = 1;
					}
					else if (x_row==1 && x_col==3)
					{
						Q[x_row][x_col][action] = -1;
					}
					else if (x_row==1 && x_col==1)
					{
						Q[x_row][x_col][action] = 0;
					}
					else

					{
						//initialize expected value of stochastic process
						double expectedValue=0;

						// iterate for x_prime to calculate expected value
						for (int x_prime_row = 0; x_prime_row < number_of_states_row; x_prime_row++)
						{
							for (int x_prime_col = 0; x_prime_col < number_of_states_col; x_prime_col++)
							{
								// Busoni, Algorithm: 2.2 Line:4
								// Q_(l+1) = EXPECTEDSUM_x_prime( f_hat(x,u,x_prime) * [reward (x,u,x_prime) + gamma * max_u_prime(Q_(l)(x_prime,u_prime) ) ]  )

								// iterate for u_prime
								double maxQ = Q[x_prime_row][x_prime_col][0];

								for (int u_prime = 0; u_prime < number_of_actions; u_prime++)
								{
									// find value of { max_u_prime( Q_(l)(x_prime,u_prime) ) }
									// here x_prime represented by (x_prime_row,x_prime_col)

									if( Q[x_prime_row][x_prime_col][u_prime] > maxQ)
										maxQ = Q[x_prime_row][x_prime_col][u_prime];
								}

								double m = Model(x_row, x_col, action, x_prime_row, x_prime_col);
								double r = Reward(x_row, x_col, action, x_prime_row, x_prime_col);

								expectedValue += m * (r + gamma * maxQ);

								//For Debugging
								//cout<<"r: "<< x_row<<" c: "<< x_col<<" a: "<< action << " r': " << x_prime_row << " c': " << x_prime_col << " m: "<< m << " r:" << r << " maxQ: " << maxQ << " val: " << expectedValue<< endl;
							}
						}

						if( abs(Q[x_row][x_col][action]-expectedValue) > diff)
						{
							//cout << "difference: " << abs(Q[x_row][x_col][action]-expectedValue) << endl;
							diff = abs( Q[x_row][x_col][action] - expectedValue);
						}

						Q[x_row][x_col][action] = expectedValue;

					}

				}


			}
		}
		//For Debugging
		//Print_Q_Value();

		if(diff<epsilon)
		{
			cout << "Stopping Iterations Diff: " << diff <<endl;
			return true;
		}

	}

  	return false;
}

/**
 *
 * @param r
 * @param c
 * @return
 */
bool GridWorld::action_north(int r, int c)
{
    // can't go north if at row 0 or if in cell (2,1)
    if ((r==0) || (r==2 && c==1))
        return false;
    return true;
}

bool GridWorld::action_east(int r, int c)
{
    // can't go east if at col 3 or if in cell (1,0)
    if ((c==4-1) || (r==1 && c==0))
        return false;
    return true;
}
bool GridWorld::action_south(int r, int c)
{
    // can't go south if at row 2 or if in cell (0,1)
    if ((r==3-1) || (r==0 && c==1))
        return false;
    return true;
}


bool GridWorld::action_west(int r, int c)
{
    // can't go west if at col 0 or if in cell (1,2)
    if ((c==0) || (r==1 && c==2))
        return false;
    return true;
}

/**
 * @brief
 *
 * @param x_row
 * @param x_col
 * @param action
 * @param x_prime_row
 * @param x_prime_col
 * @return
 */
double GridWorld::Model(int x_row, int x_col, int action, int x_prime_row, int x_prime_col)
{
	// our model is a discrete probability transition function f_hat-> busoni,Algorithm 2.2

	//action 0: North
	//action 1: East
	//action 2: South
	//action 3: West
	double pDes = probability_for_desired_action;
	double pLeft = probability_for_left_action;
	double pRight = probability_for_left_action;

	double grid[3][4] = {0};

	if(action==0)
	{
		action_north(x_row,x_col) ? grid[x_row-1][x_col] += pDes : grid[x_row][x_col] += pDes ;
		action_east(x_row,x_col) ? grid[x_row][x_col+1] += pRight : grid[x_row][x_col] += pRight ;
		action_west(x_row,x_col) ? grid[x_row][x_col-1] += pLeft : grid[x_row][x_col] += pLeft ;
	}
	if(action==1)
	{
		action_east(x_row,x_col) ? grid[x_row][x_col+1] += pDes : grid[x_row][x_col] += pDes ;
		action_south(x_row,x_col) ? grid[x_row+1][x_col] += pRight : grid[x_row][x_col] += pRight ;
		action_north(x_row,x_col) ? grid[x_row-1][x_col] += pLeft : grid[x_row][x_col] += pLeft ;
	}
	if(action==2)
	{
		action_south(x_row,x_col) ? grid[x_row+1][x_col] += pDes : grid[x_row][x_col] += pDes ;
		action_west(x_row,x_col) ? grid[x_row][x_col-1] += pRight : grid[x_row][x_col] += pRight ;
		action_east(x_row,x_col) ? grid[x_row][x_col+1] += pLeft : grid[x_row][x_col] += pLeft ;
	}
	if(action==3)
	{
		action_west(x_row,x_col) ? grid[x_row][x_col-1] += pDes : grid[x_row][x_col] += pDes ;
		action_north(x_row,x_col) ? grid[x_row-1][x_col] += pRight : grid[x_row][x_col] += pRight ;
		action_south(x_row,x_col) ? grid[x_row][x_col+1] += pLeft : grid[x_row][x_col] += pLeft ;

	}

	return grid[x_prime_row][x_prime_col];
}

double GridWorld::Reward(int x_row, int x_col, int action, int x_prime_row, int x_prime_col)
{
	return (-0.04);
}

void GridWorld::Print_Q_Value(void)
{
	cout << "Q Values:" << endl;

	for (int r = 0; r < number_of_states_row; r++)
	{
		for (int c = 0; c < number_of_states_col; c++)
		{
			for (int a = 0; a < number_of_actions; a++)
			{
				cout <<  setw(10) << setprecision(5)  << Q[r][c][a] << ":";
			}
			cout << "    ";
		}
		cout << " " << endl;
	}
}

void GridWorld::Print_V_Value(void)
{
	cout << "V Values:" << endl;

	for (int r = 0; r < number_of_states_row; r++)
	{
		for (int c = 0; c < number_of_states_col; c++)
		{
			double maxQ=-100;
			for (int a = 0; a < number_of_actions; a++)
			{
				if( Q[r][c][a] > maxQ)
				{
					maxQ = Q[r][c][a];
					Policy[r][c] = a;
				}
			}

			//cout << Q[r][c][a] << ":";
			cout <<  setw(10) << setprecision(5) << maxQ;
			cout << "    ";
		}
		cout << " " << endl;
	}
}

void GridWorld::Print_Policy(void)
{
	cout << "Policy:" << endl;

	for (int r = 0; r < number_of_states_row; r++)
	{
		for (int c = 0; c < number_of_states_col; c++)
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
}
