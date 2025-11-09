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

#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "magic/mobject.h"
#include "magic/mtable.h"

BEGIN_NAMESPACE (MagiC);

// External declarations
class OStream;
template <class type>
class PackArray;
typedef PackArray<double> Vector;

///////////////////////////////////////////////////////////////////////////////
//                         |   |               o                             //
//                         |\ /|  ___   |                                    //
//                         | V |  ___| -+- |/\ | \ /                         //
//                         | | | (   |  |  |   |  X                          //
//                         |   |  \__|   \ |   | / \                         //
///////////////////////////////////////////////////////////////////////////////

/** Mathematical matrix.
 **/
class Matrix : public PackTable<double> {
  public:
					Matrix			() : PackTable<double> () {;}
					Matrix			(int rows, int cols) : PackTable<double> (rows, cols) {}
					Matrix			(int rows, int cols, double* data);
					Matrix			(const Matrix& o) : PackTable<double> (o) {;}
					~Matrix			() {}

	void			make			(int rs, int cs);
	void			load			(const String& filename);
	void			load			(FILE* in);
	void			save			(FILE* out) const;
	
	const Matrix&	transpose		();
	const Matrix&	multiplyToSum	(double s);
	double			sum				() const;
	double			det				() const;

	void			splitVertical	(Matrix& a, Matrix& b, int column) const;
	void			splitHorizontal	(Matrix& a, Matrix& b, int column) const;

	Matrix			sub				(int row0, int row1, int col0, int col1) const;
	void			mulRowByScalar	(int row, double scalar);
	void			addRowByScalar	(int srcrow, int dstrow, double scalar);
	void			swaprows		(int row1, int row2);
	Matrix			complement		(int row, int col) const;
	void			insertColumn	(int cols);
	
	enum iters {end=-1};

	void			joinVertical	(const Matrix& a, const Vector& b);
	void			joinVertical	(const Matrix& a, const Matrix& b);
	void			joinHorizontal	(const Matrix& a, const Matrix& b);
	
	const Matrix&	operator=		(double x);
	const Matrix&	operator+=		(const Matrix& other);
	const Matrix&	operator+		(const Matrix& other) const {return Matrix(*this)+=other;}
	const Matrix&	operator+=		(double k);
	const Matrix&	operator+		(double k) const {return Matrix(*this)+=k;}
	const Matrix&	operator*=		(const Matrix& other);
	const Matrix&	operator*		(const Matrix& other) const {return Matrix(*this)*=other;}
	const Matrix&	operator*=		(double k);
	const Matrix&	operator*		(double k) const {return Matrix(*this)*=k;}
	const Matrix&	operator/		(double k) const {return Matrix(*this)*=1/k;}
	const Matrix&	operator/=		(double k) {return operator *= (1/k);}

	TextOStream&	operator>>		(TextOStream&) const;
	const Matrix&	operator= (const Matrix& other);

  private:
};

inline Matrix transpose (const Matrix& m) {
	Matrix res = m;
	res.transpose ();
	return res;
}

int solveLinear	(const Matrix& mat, const Vector& b, Vector& result);
int solveLinear	(const Matrix& augmat, Vector& result, int* nbv_set = NULL);

///////////////////////////////////////////////////////////////////////////////
//                 ----             |   |               o                    //
//                (           |     |\ /|  ___   |                           //
//                 ---  |   | |---  | V |  ___| -+- |/\ | \ /                //
//                    ) |   | |   ) | | | (   |  |  |   |  X                 //
//                ___/   \__! |__/  |   |  \__|   \ |   | / \                //
///////////////////////////////////////////////////////////////////////////////

class SubMatrix : public Matrix {
  public:
					SubMatrix		(Matrix& m, int startRow, int endRow, int startCol, int endCol);
	virtual double&	get				(int row, int col);
	// virtual double&	get				(int row, int col) const;

  protected:
	Matrix&	mrMatrix;
	int		mStartRow;
	int		mEndRow;
	int		mStartCol;
	int		mEndCol;
};

END_NAMESPACE;

#endif
