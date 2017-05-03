/*
 * ProbabilityDistribution.cpp
 *
 *  Created on: Mar 10, 2017
 *      Author: farabiahmed
 */

#include "ProbabilityDistributions/ProbabilityDistribution.hpp"


ProbabilityDistribution::ProbabilityDistribution() {

}

ProbabilityDistribution::~ProbabilityDistribution() {

}

double ProbabilityDistribution::Generate_Random_Value() {

  return rand()/(RAND_MAX+1.0);
}

