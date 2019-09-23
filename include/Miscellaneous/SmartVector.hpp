/*
 * Vector.h
 *
 *  Created on: Mar 8, 2017
 *      Author: farabiahmed
 */

#ifndef MISCELLANEOUS_SMARTVECTOR_H_
#define MISCELLANEOUS_SMARTVECTOR_H_

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <sstream>

using namespace std;

/* Considerations:
 * Law of the big three: (Resolves double free or corruption errors)
 * 	+ Destructor
 * 	+ Copy Constructor
 * 	+ Assignment Operator
 */

/*
 * Example: Simple Usage
 * 		SmartVector state(2);
 *		state.elements[0] = 5;
 *		state.elements[1] = 2;
 *		state.Print();
 */

/*
 * Example Usage 2: Operator Over-loadings
 *
 *		SmartVector vec1(10);
 *		vec1.Initialize(4.3);
 *
 *		SmartVector vec2(10);
 *		vec2.Initialize(-1.0);
 *
 *		SmartVector vec3 = 3*vec1*3 + vec2; //Result: 37.7
 *		vec3.Print();
 */

class SmartVector {
public:

	// Default Constructor
	SmartVector();

	// Custom Constructor
	SmartVector(int);

	// Copy Constructor
	SmartVector(const SmartVector& other);

	// Default Destructor
	virtual ~SmartVector();

	// Assignment Operator
	SmartVector& operator=(const SmartVector& other);

	// Comparison Operator
	const bool operator==(const SmartVector& other) const;

	// Sum Operator
	const SmartVector operator+(const SmartVector& other) const;

	// Subtraction Operator
	const SmartVector operator-(const SmartVector& other) const;

	// Scalar Multiplier Operator
	const SmartVector operator*(double scalar) const;

	// Scalar Multiplier Operator for left-hand-side operations
	// Friend because this non-member function should access the private members.
	friend SmartVector operator*(double scalar, const SmartVector& vec);

	// Prints The Elements Of The SmartVector
	void Print() const;

	// Resets the elements of vector to zero.
	void Initialize(void);

	// Resets the elements of vector to given value.
	void Initialize(double val);

	// Get the size of elements
	int size() const;

	// Calculate Magnitude of the vector
	double Magnitude() const;

	std::string Serialize() const
	{
		std::stringstream ss;
		for (int i = 0; i < dimension; ++i) {
			ss<<elements[i];

			if(i!=dimension-1)
				ss<<",";
		}
		return ss.str();
	}
	// Static Functions

	// Returns Inner Product of the input vectors
	static double InnerProduct(const SmartVector& v1, const SmartVector& v2);

	// Sum Operator
	static SmartVector Plus(const SmartVector& v1, const SmartVector& v2);

	// Combines two vectors into a single vector.
	static SmartVector Combine(const SmartVector& v1, const SmartVector& v2);

	// Splits the vector into given multiple vectors with given size n.
	static vector<SmartVector> Split(const SmartVector& vec, const int n);

	// Caution!: If you add or remove any property,
	// you need to modify constructors and assignment
	// operators.

	// Main pointer that stores vector elements
	double* elements;

	// Dimension of the vector
	int dimension;

	// Index of this vector in its space.
	int index; 	// starts from 0 if initialized.
				// otherwise -1.

};

#endif /* MISCELLANEOUS_SMARTVECTOR_H_ */
