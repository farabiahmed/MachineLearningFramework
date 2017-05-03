// CleaningRobotDeterministic.hpp
#ifndef CLEANINGROBOTDETERMINISTIC_HPP
#define CLEANINGROBOTDETERMINISTIC_HPP


#include <iostream>

using namespace std;

class CleaningRobotDeterministic {
	public:
		// only declaration in the .h unless only a short initialization list is used.

		CleaningRobotDeterministic(void); // This is the constructor declaration
		
		~CleaningRobotDeterministic(void); // This is the destructor declaration

		bool Run_Iteration();

		int Model(int, int);

		int Reward(int, int);

		void Print_Q_Value(void);

		int max_number_of_iterations;

		float gamma;

		float epsilon;

		float Q[6][2];

	private:
		int *states;

		int *actions;

		int number_of_states;

		int number_of_actions;
 };

#endif
