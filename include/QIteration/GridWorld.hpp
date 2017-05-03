// GridWorld.hpp
#ifndef GRIDWORLD_HPP
#define GRIDWORLD_HPP


#include <iostream>

using namespace std;

/*
 * Example Usage
 */

/*
GridWorld robot;

robot.Run_Iteration();

robot.Print_Q_Value();

robot.Print_V_Value();

robot.Print_Policy();
*/

class GridWorld {
	public:
		// only declaration in the .h unless only a short initialization list is used.

		GridWorld(void); // This is the default constructor declaration
		
		~GridWorld(void); // This is the destructor declaration

		bool Run_Iteration();

		double Model(int, int, int, int, int);

		double Reward(int, int, int, int, int);

		void Print_Q_Value(void);

		void Print_V_Value(void);

		void Print_Policy(void);

		double Q[3][4][4]; //3 Row, 4 Column, 4 Action

		int Policy[3][4]; //Grid World: 3 Row, 4 Column

	private:

		int number_of_states_row;

		int number_of_states_col;

		int number_of_actions;

		int max_number_of_iterations;

		double gamma;

		double epsilon;

		double probability_for_desired_action;

		double probability_for_left_action;

		double probability_for_right_action;

		bool action_north(int,int);

		bool action_east(int,int);

		bool action_south(int,int);

		bool action_west(int,int);
 };

#endif
