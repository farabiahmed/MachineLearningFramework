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
#include "Miscellaneous/UserControl.hpp"
#include "Miscellaneous/ConfigParser.hpp"

using namespace std;
/*
 *
 */
class Agent {
public:
	Agent(const ConfigParser& cfg);

	virtual ~Agent();

	virtual bool Start_Execution() = 0; ///< Main solver that runs the bellman iterations.

	void Get_Report(string filePath, string fileName);

	float Get_Score(void);

	int max_number_of_iterations;	///< Stores number of max iteration to find best policy.
	int num_of_iteration; 			///< Current status of iteration
	unsigned numberof_bellmanupdate; ///< Current status of bellman update counter

	vector<SmartVector> states;  ///< Stores the states of the environment to increase speed
	 	 	 	 	 	 	 	 ///< of calculation.

	string Name;	///< It stores the name of agent to increase readability and awareness.
    				///< Such as Qiteration, TrajectoryBased etc.

	string log_file_path;


	void Get_Cumulative_Rewards(unsigned numberof_bellmanupdate);  	///< It does the simulation for predefined times
																	///< in a loop and fills the "rewards_cumulative".


protected:

	Environment* environment;

	Representation* valueFunction;

	UserControl& userControl;

	std::fstream logger_agentStats;
	std::fstream logger_simResults;

	pair<unsigned,double> Simulate(void); 		///< Simulates the agent from an random initial point through
												///< calculated last policy.

	pair<unsigned,double> FitnessValue(SmartVector state);

	vector< pair<int,vector<double>> > rewards_cumulative;	///< Stores the rewards that collected during
															///< each iteration. Then printed out to csv file
															///< to be shown by python plotting libraries.
															///< pair: <numberof_bellmanupdate, rewards_for_that_bellmanupdate>
															///< rewards_cumulative is a list of pairs given above.

	unsigned number_of_simulations;				///< Holds the number of simulations that needs to
												///< be done for performance evaluation.

	unsigned max_steps_in_simulation;			///< It prevents the agent to stuck in a loop in the environment
	 	 	 	 	 	 	 	 	 	 	 	///< It is recommended that to keep it high to know whether it
	 	 	 	 	 	 	 	 	 	 	 	///< is stucked in the cumulative rewards plot.

	unsigned bellman_stride_forsimulation;			///< Defines how many bellman updates should be discarded to
											    ///< make simulation to plot the performance of agent.


};

#endif /* AGENTS_AGENT_HPP_ */
