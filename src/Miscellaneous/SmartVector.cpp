/*
 * Vector.cpp
 *
 *  Created on: Mar 8, 2017
 *      Author: farabiahmed
 */

#include "Miscellaneous/SmartVector.hpp"


SmartVector::SmartVector(int N) {
	// TODO Auto-generated constructor stub
	dimension = N;
	elements = new double[dimension];
	index = -1; // initially undefined.
}

SmartVector::SmartVector() {
	// TODO Auto-generated constructor stub
	dimension = 0;
	elements = nullptr;
	index = -1; // initially undefined.
}

// Copy Constructor (Rule Of Three #2)
SmartVector::SmartVector(const SmartVector& other) :
		elements(new double[other.dimension]),dimension(other.dimension),index(other.index)
{
	// copy element by element
	for (int var = 0; var < dimension; ++var) {
		elements[var] = other.elements[var];
	}
}

// Assignment Operator (Rule Of Three #3)
SmartVector& SmartVector::operator=(const SmartVector& other)
{
	// release memory first
	delete[] elements;
	elements = nullptr;

	this->dimension = other.dimension;

	this->index = other.index;

	elements = new double[dimension];

	// copy element by element
	for (int var = 0; var < dimension; ++var) {
		elements[var] = other.elements[var];
	}

	return *this;
}

const bool SmartVector::operator==(const SmartVector& other) const
{
	// This operator just consider elements, not the index
	// that indicates the sequence number its set that is belonging to.

	if(dimension!=other.dimension)
	{
		throw std::invalid_argument( "vector dimensions mismatched." );
	}

	for (int i = 0; i < dimension; ++i) {
		if(elements[i]!=other.elements[i])
			return false;
	}

	return true;
}


const SmartVector& SmartVector::operator+(const SmartVector& other) const
{
	if(dimension!=other.dimension)
	{
		throw std::invalid_argument( "vector dimensions mismatched." );
	}

	for (int i = 0; i < dimension; ++i) {
		elements[i] += other.elements[i];
	}

	return *this;
}

const SmartVector& SmartVector::operator*(double scalar) const
{
	for (int i = 0; i < dimension; ++i) {
		elements[i] *= scalar;
	}

	return *this;
}

// Non-Member Function to handle left-hand-side multiplication
SmartVector operator*(double scalar, const SmartVector& vec)
{
	SmartVector result(vec.dimension);

	for (int i = 0; i < vec.dimension; ++i) {
		result.elements[i] = vec.elements[i] * scalar;
	}

	return result;
}

void SmartVector::Initialize(void)
{
	for (int i = 0; i < dimension; ++i) {
		elements[i] = 0;
	}
}

void SmartVector::Initialize(double val)
{
	for (int i = 0; i < dimension; ++i) {
		elements[i] = val;
	}
}

int SmartVector::size() const
{
	return dimension;
}

void SmartVector::Print() const
{
	// Inform user that it is the begining.
	cout<< endl <<"Printing Elements of The Vector:"<<endl;

	// Print Index of vector in its own space
	cout<<"Index in its space: " << index << endl;

	// Print each element of the vector
	for (int var = 0; var < dimension; ++var) {
		cout<<"Element " << var << " :" << elements[var] << endl;
	}

	// Inform user that it is the end.
	cout<<"End of vector."<<endl<<endl;
}

// Destructor (Rule Of Three #1)
SmartVector::~SmartVector() {

	// matching pair of allocators
	//	+ malloc/free
	//	+ new/delete
	//	+ new[]/delete[]

	// release memory
	delete[] elements;
}


double SmartVector::InnerProduct(const SmartVector& v1, const SmartVector& v2)
{
	double sum=0.0;

	if(v1.dimension!=v2.dimension)
	{
		throw std::invalid_argument( "vector dimensions mismatched." );
	}

	for (int i = 0; i < v1.dimension; ++i) {
		sum += v1.elements[i] * v2.elements[i];
	}

	return sum;
}

SmartVector SmartVector::Plus(const SmartVector& v1, const SmartVector& v2)
{
	SmartVector sumVec(v1.dimension);

	if(v1.dimension!=v2.dimension)
	{
		throw std::invalid_argument( "vector dimensions mismatched." );
	}

	for (int i = 0; i < v1.dimension; ++i) {
		sumVec.elements[i] = v1.elements[i] + v2.elements[i];
	}

	return sumVec;
}
