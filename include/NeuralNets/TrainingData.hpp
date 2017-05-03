/*
 * TrainingData.hpp
 *
 *  Created on: Apr 6, 2017
 *      Author: farabiahmed
 */

#ifndef NEURALNETS_TRAININGDATA_HPP_
#define NEURALNETS_TRAININGDATA_HPP_

#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>

using namespace std;
/*
 *
 */
class TrainingData {
public:
public:
    TrainingData(const string filename);
    virtual ~TrainingData();
    bool isEof(void) { return m_trainingDataFile.eof(); }
    void getTopology(vector<unsigned> &topology);

    // Returns the number of input values read from the file:
    unsigned getNextInputs(vector<double> &inputVals);
    unsigned getTargetOutputs(vector<double> &targetOutputVals);

private:
    ifstream m_trainingDataFile;

};

#endif /* NEURALNETS_TRAININGDATA_HPP_ */
