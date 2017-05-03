/*
 * QIteration.hpp
 *
 *  Created on: Mar 20, 2017
 *      Author: farabiahmed
 */

#ifndef AGENTS_QITERATION_HPP_
#define AGENTS_QITERATION_HPP_

#include <cmath>
#include "Agents/Agent.hpp"
#include "Miscellaneous/ConfigParser.hpp"

/*
 *
 */
class QIteration :  public Agent {
public:
	QIteration(const Environment* env, const Representation* rep, const ConfigParser& cfg);
	virtual ~QIteration();
	bool Start_Execution();
	void Test(void);

private:

	double gamma;

	double epsilon;
};

#endif /* AGENTS_QITERATION_HPP_ */
