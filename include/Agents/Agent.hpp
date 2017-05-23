/*
 * Agent.hpp
 *
 *  Created on: Mar 20, 2017
 *      Author: farabiahmed
 */

#ifndef AGENTS_AGENT_HPP_
#define AGENTS_AGENT_HPP_

#include <fstream> // file write for datalog
#include <cstdlib> // system command
#include "Environments/Environment.hpp"
#include "Representations/Representation.hpp"
#include "ProbabilityDistributions/ProbabilityDistribution.hpp"
#include "ProbabilityDistributions/DiscreteDistribution.hpp"

using namespace std;
/*
 *
 */
class Agent {
public:
	Agent();

	virtual ~Agent();

	virtual bool Start_Execution() = 0; ///< Main solver that runs the bellman iterations.

	void Get_Report(string filePath, string fileName);

	int max_number_of_iterations;	///< Stores number of max iteration to find best policy.

	vector<SmartVector> states;  ///< Stores the states of the environment to increase speed
	 	 	 	 	 	 	 	 ///< of calculation.

	string Name;	///< It stores the name of agent to increase readability and awareness.
    				///< Such as Qiteration, TrajectoryBased etc.

protected:

	Environment* environment;

	Representation* valueFunction;

	void Get_Cumulative_Rewards(void);  ///< It does the simulation for predefined times
											///< in a loop and fills the "rewards_cumulative".

	double Simulate(void); 		///< Simulates the agent from an random initial point through
								///< calculated last policy.

	vector<vector<double>> rewards_cumulative;	///< Stores the rewards that collected during
												///< each iteration. Then printed out to csv file
												///< to be shown by python plotting libraries.

	unsigned number_of_simulations;///< Holds the number of simulations that needs to
												///< be done for performance evaluation.
};

#endif /* AGENTS_AGENT_HPP_ */
