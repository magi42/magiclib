#include <stdlib.h>
#include "magic/mmath.h"
#include "magic/mcoord.h"
#include "magic/mmagisupp.h"

#ifndef RAND_MAX
#define RAND_MAX 32767
#endif

BEGIN_NAMESPACE (MagiC);

double sum (const Vector& x) {
	double res = 0.0;
	for (int i=0; i<x.size(); i++)
		res += x[i];
	return res;
}

double min (const Vector& x) {
	double min = UNDEFINED_FLOAT;
	for (int i=0; i<x.size(); i++)
		if (!is_undef(x[i]) && x[i]<min)
			min = x[i];
	return min;
}

int minIndex (const Vector& x) {
	double min = UNDEFINED_FLOAT;
	int minI=0;
	for (int i=0; i<x.size(); i++)
		if (!is_undef(x[i]) && x[i]<min) {
			min = x[i];
			minI = i;
		}
	return minI;
}

double max (const Vector& x) {
	double max = -UNDEFINED_FLOAT;
	for (int i=0; i<x.size(); i++)
		if (!is_undef(x[i]) && x[i]>max)
			max = x[i];
	return max;
}

int maxIndex (const Vector& x) {
	double max = -UNDEFINED_FLOAT;
	int maxI=0;
	for (int i=0; i<x.size(); i++)
		if (!is_undef(x[i]) && x[i]>max) {
			max = x[i];
			maxI = i;
		}
	return maxI;
}

double avg (const Vector& x) {
	return sum (x) / x.size();
}

double stddev (const Vector& x) {
	double average = avg (x);
	double res = 0.0;
	for (int i=0; i<x.size(); i++)
		res += sqr (x[i]-average);
	return sqrt (res/(x.size()+1));
}

double stdmerr (const Vector& x) {
	return stddev(x)/sqrt(double(x.size()));
}

Vector histogram (const Vector& x, int n) {
	Vector result;
	
	// Init histogram
	result.make (n);
	for (int i=0; i<result.size(); i++)
		result[i] = 0.0;

	// Find minima and maxima
	double mi=min(x), ma=max(x);
	double mul = double(n-1)/(ma-mi);
	
	// Make histogram
	int pos=0;
	for (int i=0; i<x.size(); i++) {
		pos = int(mul*(x[i]-mi));
		result[pos] = result[pos] + 1.0;
	}

	/*
	// Calculate means
	int meanrange=2;
	Vector result2 (n);
	for (int i=0; i<result.size(); i++) {
		int values=0;
		double sum = 0.0;
		for (int m=-meanrange; m<=meanrange; m++)
			if (i+m>=0 && i+m<result.size()) {
				sum += result[i+m];
				values++;
			}
		result2[i] = sum/double(values);
	}
	*/

	return result;
}

Vector histogramInRange (const Vector& x, int n, double mi, double ma, int window)
{
	Vector result;
	
	// Init histogram
	result.make (n);
	for (int i=0; i<result.size(); i++)
		result[i] = 0.0;

	double mul = double(n-1)/(ma-mi);

	// Make histogram
	int pos=0;
	for (int i=0; i<x.size(); i++) {
		pos = int(mul*(x[i]-mi));
		if (pos >= 0 && pos<n) {
			int wstart = (pos-window)<0? 0:pos-window;
			int wend   = (pos+window)>=n? n-1:pos+window;
			for (int w =  wstart; w<=wend; w++)
				result[w] = result[w] + 1.0;
		}
	}

	return result;
}

void add (Vector& x, double a) {
	for (int i=0; i<x.size(); i++)
		x[i] += a;
}

void multiply (Vector& x, double m) {
	for (int i=0; i<x.size(); i++)
		x[i] *= m;
}

void multiplyToUnity (Vector& x) {
	double xsum = sum (x);
	if (xsum==0)
		add (x, 1.0/x.size());
	else
		multiply (x, 1/xsum);
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                    ___                    |  __  ___                     //
//                   /   \                   | /  \ |  \                    //
//                   |      __   __  |/\  ---|   _/ |   |                   //
//                   |     /  \ /  \ |   (   |  /   |   |                   //
//                   \___/ \__/ \__/ |    ---| /___ |__/                    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

void Coord2D::copy (const Coord3D& o) {
	x=o.x;
	y=o.y;
}

/*
double quadatan (double y, double x) {
	double at=atan (y/x);
	
	// Tuodaan oikeaan kvadranttiin
	if (x<0 && y>0) at+=M_PI;
	if (x<0 && y<0) at+=M_PI;
	if (x>0 && y<0) at+=2*M_PI;
	
	return at;
}
*/

END_NAMESPACE;

