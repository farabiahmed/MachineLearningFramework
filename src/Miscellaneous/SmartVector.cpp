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
	// Check for self-assignment
	if (this != &other)
	{
		// release memory first
		delete[] elements;
		elements = nullptr;

		// Start To Copy
		this->dimension = other.dimension;

		this->index = other.index;

		elements = new double[dimension];

		// copy element by element
		for (int var = 0; var < dimension; ++var) {
			elements[var] = other.elements[var];
		}
	}
	return *this;
}

const bool SmartVector::operator==(const SmartVector& other) const
{
	// This operator just consider elements, not the index
	// that indicates the sequence number its set that is belonging to.

	if(dimension!=other.dimension)
	{
		throw std::invalid_argument( "SmartVector operator== vector dimensions mismatched." );
	}

	for (int i = 0; i < dimension; ++i) {
		if(elements[i]!=other.elements[i])
			return false;
	}

	return true;
}


const SmartVector SmartVector::operator+(const SmartVector& other) const
{
	if(dimension!=other.dimension)
	{
		throw std::invalid_argument( "vector dimensions mismatched." );
	}

	SmartVector vec(dimension);

	for (int i = 0; i < dimension; ++i) {
		vec.elements[i] = elements[i] + other.elements[i];
	}

	return vec;
}

const SmartVector SmartVector::operator-(const SmartVector& other) const
{
	if(dimension!=other.dimension)
	{
		throw std::invalid_argument( "vector dimensions mismatched." );
	}

	SmartVector vec(dimension);

	for (int i = 0; i < dimension; ++i) {
		vec.elements[i] = elements[i] - other.elements[i];
	}

	return vec;
}

const SmartVector SmartVector::operator*(double scalar) const
{
	SmartVector result(dimension);

	for (int i = 0; i < dimension; ++i) {
		result.elements[i] = elements[i] * scalar;
	}

	return result;
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

double SmartVector::Magnitude() const
{
	double sum;

	for (int i = 0; i < dimension; i++)
	{
		sum += elements[i]*elements[i];
	}

	return sqrt(sum);
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

SmartVector SmartVector::Combine(const SmartVector& v1, const SmartVector& v2)
{
	SmartVector comVec(v1.size()+v2.size());

	for (int i = 0; i < v1.size(); ++i) {
		comVec.elements[i] = v1.elements[i];
	}

	for (int i = 0; i < v2.size(); ++i) {
		comVec.elements[i+v1.size()] = v2.elements[i];
	}

	return comVec;
}

vector<SmartVector> SmartVector::Split(const SmartVector& vec, const int n)
{
	if(vec.dimension % n != 0)
	{
		throw std::invalid_argument( "SmartVector split: vector dimensions mismatched." );
	}

	// Get the size of the vectors that will be produced.
	int new_size = vec.dimension / n;

	vector<SmartVector> ret;

	for (int i = 0; i < vec.size(); i+=new_size)
	{
		SmartVector s(new_size);

		for (int j = 0; j < new_size; ++j) {
			s.elements[j] = vec.elements[i+j];
		}

		ret.push_back(s);
	}

	return ret;
}
