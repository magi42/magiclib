/***************************************************************************
    copyright            : (C) 2000 by Marko Grönroos
    email                : magi@iki.fi
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 *
 **/

#ifndef __MATH_HPP__
#define __MATH_HPP__

#include <magic/mobject.h>
#include <magic/mpackarray.h>

// In SunOS/Solaris there is a fp-exception class in math.h.... SHINY BLOODY IDIOTS!
//#ifdef __P
//#undef __P
//#endif
//#ifdef SunOS
//#define exception exception_kludge
//#include <math.h>
//#undef exception
//#else
#include <math.h>
//#endif

BEGIN_NAMESPACE (MagiC);


/** Returns a random integer value in range 0..range-1.
 **/
int		rnd			(int range);

/** Returns a double-float random value between 0 and 1.
 **/
double	frnd		();

/** Returns a normally distributed random value with the given
 *  standard deviation.
 **/
double	gaussrnd	(double stdv);

/** Square x^2.
 **/
inline double	sqr	(double x) {return x*x;}

//double quadatan (double y, double x);

template <class T>
inline T sqr (T x) {
	return x*x;
}

/** Standard sigmoid (logistic) function.
 **/
inline double sigmoid (double x) {return 1/(1+exp(-x));}

// |x|
//inline double	abs	(double a) {
//	return a>=0? a:-a;
//}

/** Simple swap operation on doubles.
 **/
inline void swap (double& x, double& y) {double tmp=x; x=y; y=tmp;}

#define UNDEFINED_FLOAT 1.2345E30

/** Retuns if the given value has an "undefined" value.
 **/
inline bool is_undef (double x) {
	return x>=1.2345E29;
}

/** Vector is just an alias for the @ref PackArray<double>, not even a
 *  subclass.
 **/
typedef PackArray<double>	Vector;

/** Sum of values in @ref Vector  x. */
double	sum			(const Vector& x);
/** Lowest value in @ref Vector  x. */
double	min			(const Vector& x);
/** The index of the lowest value in @ref Vector  x. */
int		minIndex	(const Vector& x);
/** Largest value in @ref Vector  x. */
double	max			(const Vector& x);
/** The index of the largest value in @ref Vector  x. */
int		maxIndex	(const Vector& x);
/** Average of the values in @ref Vector  x. */
double	avg			(const Vector& x);
/** Standard deviation of the values in @ref Vector  x. */
double	stddev		(const Vector& x);
/** Standard error of the values in @ref Vector  x. */
double	stdmerr		(const Vector& x);

/** Creates a histogram with n slots of the values in the @ref Vector
 *  .
**/
Vector	histogram	(const Vector& x, int n);

Vector histogramInRange (const Vector& x, int n, double mi, double ma, int window);


/** Adds a value to the @ref Vector . */
void	add (Vector& x, double m);

/** Multiplies the values in the @ref Vector with a coefficient.
 **/
void	multiply (Vector& x, double m);

/** Multiplies the values in the @ref Vector so that they sum to 1.0.
 **/
void	multiplyToUnity (Vector& x);

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** Threshold for rounding values in certain functions. */
#ifndef ROUND_DOUBLE_THRESHOLD
#define ROUND_DOUBLE_THRESHOLD 1e-15
#endif

inline bool isRoundZero (double x) {
	return fabs(x) < ROUND_DOUBLE_THRESHOLD;
}

END_NAMESPACE;

#endif
