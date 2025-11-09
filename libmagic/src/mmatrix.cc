/***************************************************************************
 *   This file is part of the MagiC++ library.                             *
 *                                                                         *
 *   Copyright (C) 1998-2002 Marko Grönroos <magi@iki.fi>                  *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 *  This library is free software; you can redistribute it and/or          *
 *  modify it under the terms of the GNU Library General Public            *
 *  License as published by the Free Software Foundation; either           *
 *  version 2 of the License, or (at your option) any later version.       *
 *                                                                         *
 *  This library is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *  Library General Public License for more details.                       *
 *                                                                         *
 *  You should have received a copy of the GNU Library General Public      *
 *  License along with this library; see the file COPYING.LIB.  If         *
 *  not, write to the Free Software Foundation, Inc., 59 Temple Place      *
 *  - Suite 330, Boston, MA 02111-1307, USA.                               *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <magic/mmath.h>
#include <magic/mmatrix.h>
#include <magic/mstream.h>
#include <magic/mtextstream.h>
#include <magic/mlist.h>

BEGIN_NAMESPACE (MagiC);

///////////////////////////////////////////////////////////////////////////////
//                         |   |               o                             //
//                         |\ /|  ___   |                                    //
//                         | V |  ___| -+- |/\ | \ /                         //
//                         | | | (   |  |  |   |  X                          //
//                         |   |  \__|   \ |   | / \                         //
///////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 *
 ******************************************************************************/
Matrix::Matrix (
	int       rows,
	int       cols,
	double*   pData)
		: PackTable<double> (rows, cols)
{
	memcpy (mData, pData, rows*cols*sizeof(double));
}

void Matrix::make (int rs, int cs)
{
	PackTable<double>::make (rs, cs);
	operator= (0.0);
}

/*******************************************************************************
 * Load matrix from file.
 *
 * Columns have to be whitespace-separated, rows newline-separated.
 ******************************************************************************/
void Matrix::load (const String& filename)
{
	FILE* in = fopen (filename, "r");
	if (!in)
		throw file_not_found (format("Matrix file '%s' not found", (CONSTR)filename));
	load (in);
	fclose (in);
}

/*******************************************************************************
 * Load matrix from stream.
 *
 * Columns have to be whitespace-separated, rows newline-separated.
 ******************************************************************************/
void Matrix::load (FILE* in) {
	List<Vector*> rows;		// Will contain the rows in the input file

	// Read the datafile
	String buffer;
	String item;		// One numeric or text item
	item.reserve (20);

	// Read one line at a time
	int rowCnt = 0;
	while (fgetS (in, buffer) != -1) {
	 	buffer.chop(); // Remove trailing newline
		Vector* prow = new Vector (cols);
		int itemIndex=0;	// Index of the above item within the row
		for (uint i=0; i<buffer.length()+1; i++) {
		 	if (i==buffer.length() || buffer[i]==' ' || buffer[i]=='\t') {
			 	if (itemIndex<cols)
				 	(*prow)[itemIndex++] = (item=="x")? UNDEFINED_FLOAT : item.toDouble();
				item = "";
			} else
			 	item += buffer[i];
		}
		rows.add (prow);
		rowCnt++;
 	}

	// Adjust our size
 	make (rowCnt,cols);

	// Put the pattern list into the set
	int r=0;
	for (ListIter<Vector*> l (rows); !l.exhausted(); l.next(), r++) {
	 	for (int c=0; c<cols; c++)
		 	get(r,c) = (*l.get())[c];
 	}
}

/*******************************************************************************
 *
 ******************************************************************************/
void Matrix::save (FILE* out) const {
	for (int r=0; r<rows; r++) {
		for (int c=0; c<cols; c++)
			fprintf (out, "%g ", get(r,c));
		fprintf (out, "\n");
	}
}

/*******************************************************************************
 *
 ******************************************************************************/
const Matrix& Matrix::transpose () {
	for (int i=0; i<rows; i++)
		for (int j=i; j<cols; j++)
			swap (get(i,j), get(j,i));
	return *this;
}

/*******************************************************************************
 * Makes the sum of the matrix a given value.
 ******************************************************************************/
const Matrix& Matrix::multiplyToSum (double s) {
	double cs = sum ();
	ASSERTWITH (fabs(cs)>1E-10, "Tried to normalize a zero matrix");

	operator*= (1.0/cs);
	return *this;
}

/*******************************************************************************
 *
 ******************************************************************************/
double Matrix::sum () const
{
	int size=rows*cols;
	double res=0.0;
	for (int i=0; i<size; i++)
		res += mData[i];
	return res;
}

/*******************************************************************************
 * Computes the determinant of the matrix.
 ******************************************************************************/
double Matrix::det () const
{
	if (rows != cols || rows<2)
		return 0;

	if (rows == 2)
		return mData[0]*mData[3] - mData[1]*mData[2];

	// Compute determinant of a sub-matrix

	// Find a row with non-zero first column
	int rownon0;
	for (rownon0=0; rownon0<rows; rownon0++)
		if (get(rownon0,0) != 0)
			break;

	if (rownon0 == rows)
		return 0;

	// Prepare for taking complement by making a column with
	// zeroes, except for one row.
	Matrix modified (*this);
	double scale_to = get(rownon0, 0);
	for (int row=0; row<rows; row++)
		if (row != rownon0 && get(row,0)!=0)
			modified.addRowByScalar (rownon0, row, -get(row,0)/scale_to);

	// Create complement
	Matrix cmpl = modified.complement (rownon0, 0);

	// Compute determinant of the complement, and multiply it by the
	// nonzero element, and the appropriate sign.
	return get(rownon0, 0) * cmpl.det () * (1-(rownon0%2)*2);
}

/*******************************************************************************
 * Forms the complement of the matrix by the given element
 ******************************************************************************/
Matrix Matrix::complement (int row, int col) const
{
	ASSERT (rows>1 && rows==cols);

	Matrix result (rows-1, cols-1);

	for (int i=0, trgrow=0; i<rows; i++)
		if (i != row) {
			for (int j=0, trgcol=0; j<cols; j++)
				if (j!=col)
					result.get(trgrow, trgcol++) = get (i,j);
			trgrow++;
		}

	return result;
}

/*******************************************************************************
 * Multiplies a row by a scalar.
 ******************************************************************************/
void Matrix::mulRowByScalar (int row, double scalar)
{
	for (int i=0; i<cols; i++)
		get(row,i) *= scalar;
}

/*******************************************************************************
 * Multiplies a row by a scalar and adds it to another row.
 *
 * Rounds values to zero if they are below threshold ROUND_TO_ZERO_THRESHOLD.
 ******************************************************************************/
void Matrix::addRowByScalar (int srcrow, int dstrow, double scalar)
{
	for (int i=0; i<cols; i++) {
		get(dstrow,i) += scalar*get(srcrow,i);

		// Round to zero
		if (isRoundZero (get(dstrow,i)))
			get(dstrow,i) = 0;
	}
}

/*******************************************************************************
 * Exchanges the values in two rows
 ******************************************************************************/
void Matrix::swaprows (int row1, int row2)
{
	double tmp [cols];
	int    collen = cols * sizeof(double);
	memcpy (&tmp, mData + row1*cols, collen);
	memcpy (mData + row1*cols, mData + row2*cols, collen);
	memcpy (mData + row2*cols, &tmp, collen);
}

/*******************************************************************************
 * Sets the entire matrix to the given value.
 ******************************************************************************/
const Matrix& Matrix::operator= (double x) {
	int size=rows*cols;
	for (int i=0; i<size; i++)
		mData[i] = x;
	return *this;
}

/*******************************************************************************
 * Matrix copy operation.
 ******************************************************************************/
const Matrix& Matrix::operator= (const Matrix& orig) {
	make (orig.rows, orig.cols);
	int size=rows*cols;
	for (int i=0; i<size; i++)
		mData[i]= orig.mData[i];
	return *this;
}

/*******************************************************************************
 * Adds the other matrix to the matrix.
 ******************************************************************************/
const Matrix& Matrix::operator+= (const Matrix& other) {
	int size=rows*cols;
	for (int i=0; i<size; i++)
		mData[i] += other.mData[i];
	return *this;
}

/*******************************************************************************
 * Adds a value to all the elements in the matrix.
 ******************************************************************************/
const Matrix& Matrix::operator+= (double x) {
	int size=rows*cols;
	for (int i=0; i<size; i++)
		mData[i] += x;
	return *this;
}

/*******************************************************************************
 * Element-by-element multiplication by another matrix
 ******************************************************************************/
const Matrix& Matrix::operator*= (const Matrix& other) {
	int size=rows*cols;
	for (int i=0; i<size; i++)
		mData[i] *= other.mData[i];
	return *this;
}

/*******************************************************************************
 * Scales the elements in the matrix with the given value.
 ******************************************************************************/
const Matrix& Matrix::operator*= (double x) {
	int size=rows*cols;
	for (int i=0; i<size; i++)
		mData[i] *= x;
	return *this;
}

/*******************************************************************************
 *
 ******************************************************************************/
TextOStream& Matrix::operator>> (TextOStream& out) const
{
	out << "{";
	for (int i=0; i<rows; i++) {
		if (i > 0)
			out << ' ';
		out << '{';
		for (int j=0; j < cols; j++) {
			out.printf ("%2f", get (i,j));
		    if (j < cols-1)
				out << ',';
		}
		out << '}';
		if (i < rows-1)
			out << '\n';
	}
	out << "}\n";
	return out;
}

/*******************************************************************************
 * Splits the matrix column-wise to the given two matrices.
 ******************************************************************************/
void Matrix::splitVertical (Matrix& a, Matrix& b, int column) const {
	a.make (rows, column);
	b.make (rows, cols-column);
	for (int r=0; r<rows; r++) {
		for (int c=0; c<column; c++)
			a.get(r,c) = get(r,c);
		for (int c=0; c<cols-column; c++)
			b.get(r,c) = get(r,c+column);
	}
}

/*******************************************************************************
 * Splits the matrix row-wise to the given two matrices.
 ******************************************************************************/
void Matrix::splitHorizontal (Matrix& a, Matrix& b, int row) const {
	a.make (row, cols);
	b.make (rows-row, cols);
	for (int c=0; c<cols; c++) {
		for (int r=0; r<row; r++)
			a.get(r,c) = get(r,c);
		for (int r=0; r<rows-row; r++)
			b.get(r,c) = get(r+row,c);
	}
}

/*******************************************************************************
 * Appends a vertical vector to matrix
 *
 * The vector must have length equal to number of rows in the matrix.
 ******************************************************************************/
void Matrix::joinVertical (const Matrix& a, const Vector& b)
{
	ASSERT (a.rows == b.size ());

	make (a.rows, a.cols+1);

	int collen  = cols*sizeof(double);
	int acollen = a.cols*sizeof(double);
	for (int r=0; r<rows; r++) {
		// Copy matrix row
		memcpy (mData + r*collen, a.mData + r*acollen, acollen);

		// Copy vector element
		mData[r*cols + cols-1] = b[r];
	}
}

/*******************************************************************************
 * Appends two matrices vertically.
 *
 * The number of rows in both matrices must be equal.
 ******************************************************************************/
void Matrix::joinVertical (const Matrix& a, const Matrix& b) {
	ASSERT (a.rows == b.rows);
	make (a.rows, a.cols + b.cols);
	for (int r=0; r<rows; r++) {
		for (int c=0; c<a.cols; c++)
			get(r,c) = a.get(r,c);
		for (int c=0; c<b.cols; c++)
			get(r,c+a.cols) = a.get(r,c);
	}
}

/*******************************************************************************
 * Appends two matrices horizontally.
 *
 * The number of columns in both matrices must be equal.
 ******************************************************************************/
void Matrix::joinHorizontal (const Matrix& a, const Matrix& b) {
	ASSERT (a.cols == b.cols);
	make (a.rows+b.rows, a.cols);
	for (int c=0; c<cols; c++) {
		for (int r=0; r<a.rows; r++)
			get(r,c) = a.get(r,c);
		for (int r=0; r<b.rows; r++)
			get(r+a.rows,c) = a.get(r,c);
	}
}

/*******************************************************************************
 * Returns submatrix, as defined by the arguments.
 ******************************************************************************/
Matrix Matrix::sub (
	int row0, /**< First row to be included.                                            */
	int row1, /**< Last row to be included, or -1 for the last row in source matrix.    */
	int col0, /**< First column to be included.                                         */
	int col1  /**< Last column to be included, or -1 for the last row in source matrix. */) const
{
	ASSERT (row0>=0 && row0<rows);
	ASSERT ((row1>=-1 && row1<rows) || row1==-1);
	ASSERT (col0>=0 && col0<cols);
	ASSERT ((col1>=-1 && col1<cols) || col1==-1);

	if (row1==-1)
		row1 = rows-1;
	if (col1==-1)
		col1 = cols-1;

	Matrix result;
	result.make (row1-row0+1, col1-col0+1);
	for (int r=0; r<result.rows; r++)
		for (int c=0; c<result.cols; c++)
			result.get(r,c) = (*this).get(r+row0, c+col0);
	return result;
}

/*******************************************************************************
 *
 ******************************************************************************/
void Matrix::insertColumn (int cols) {
	
}

/*
SubMatrix::SubMatrix (Matrix& m, int sr, int sc, int er, int ec) : matrix (m) {
	ASSERT (sr>0 && sc>0 && er>0 && ec>0);
	ASSERT (sr<m.rows);
	ASSERT (sc<m.cols);
	ASSERT (er<m.rows);
	ASSERT (ec<m.cols);
	ASSERT (sr<=er);
	ASSERT (sc<=ec);
	srow=sr, scol=sc, erow=er, ecol=ec;
	rows = erow-srow+1;
	cols = ecol-scol+1;

}

double& SubMatrix::get (int row, int col) {
	return matrix.PackTable<double>::get (row-srow, col-scol);
}
*/

/*******************************************************************************
 * Solves linear equation.
 ******************************************************************************/
int solveLinear		(const Matrix& mat, const Vector& b, Vector& result)
{
	// Create augmented matrix
	Matrix augmat (mat);
	augmat.joinVertical (mat, b);
	
	return solveLinear (augmat, result);
}

/*******************************************************************************
 * Solves linear equation represented as an augmented matrix.
 *
 * Uses the Gauss-Jordan method. The solution will be stored in the
 * "result" vector.
 *
 * If the basic variables set nbv_set is given, these variables in
 * the problem are set to be 0, and the linear equation is solved for
 * the remaining, basic variables.
 *
 * @return 0 if solution was found, nonzero otherwise
 ******************************************************************************/
int solveLinear	(const Matrix& augmat_orig, Vector& result, int* bv_set)
{
	int bv_count = 0;
	
	// Auxiliary augmented matrix which we can manipulate
	// with Elementary Row Operations
	Matrix augmat (augmat_orig);

	// Perform the actual Gauss-Jordan method
	for (int col=0, row=0; col<augmat.cols-1 && row<augmat.rows; col++)
		// Work only on basic variables
		if (!bv_set || bv_set[col]) {
		
			// Ensure that the top-left element is not 0
			if (augmat.get(row,col) == 0) {
				// Find row with non-zero top-left element
				bool found = false;
				for (int i=row+1; i<augmat.rows; i++)
					if (augmat.get(i,col) != 0) {
						augmat.swaprows (row, i);
						found = true;
					}
				
				if (!found)
					return 1; // Unsolvable
			}
			
			// Scale the top-left element to 1
			augmat.mulRowByScalar (row, 1/augmat.get(row,col));
			augmat.get(row,col) = 1; // In case of rounding errors
			
			// Make the column below the position to begin with 0
			for (int i=row+1; i<augmat.rows; i++)
				if (augmat.get(i,col) != 0) {
					augmat.addRowByScalar (row, i, -augmat.get(i,col));
					augmat.get(i,col) = 0; // In case of rounding errors
				}

			row++;
		}
		else
			// Count the number of nonbasic variables in the BV set
			bv_count++;

	// If rows > cols-1, the bottom columns must be 0
	if (augmat.rows > augmat.cols - bv_count - 1)
		for (int i=augmat.cols - bv_count - 1; i<augmat.rows; i++)
			if (!bv_set || bv_set[i])
				if (!isRoundZero (augmat.get(i,augmat.cols - bv_count - 1)))
					return 2; // Unsolvable

	// Go back up and solve the variables above
	for (int col=augmat.cols-2, row=augmat.rows-1; col>=0; col--)
		if (!bv_set || bv_set[col]) {
			// Use this solution to fix the column above to 0
			for (int i=row-1; i>=0; i--)
				if (augmat.get(i,col) != 0)
					augmat.addRowByScalar (row, i, -augmat.get(i,col));

			row--;
		}

	// Store the result
	result.make (augmat.cols-1);
	for (int var=0, row=0; var<result.size(); var++)
		if (!bv_set || bv_set[var]) // BV
			result[var] = augmat.get (row++, augmat.cols-1);
		else // BV
			result[var] = 0.0;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//                 ----             |   |               o                    //
//                (           |     |\ /|  ___   |                           //
//                 ---  |   | |---  | V |  ___| -+- |/\ | \ /                //
//                    ) |   | |   ) | | | (   |  |  |   |  X                 //
//                ___/   \__! |__/  |   |  \__|   \ |   | / \                //
///////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * Creates a virtual submatrix of a matrix.
 *
 * The full matrix is referenced and may not be destroyed before this submatrix.
 ******************************************************************************/
SubMatrix::SubMatrix (
	Matrix& m,    /**< Matrix to be virtually submatrixed. */
	int startRow, /**< Start row of the submatrix. */
	int endRow,   /**< End row of the submatrix, or negative for counting from the end. */
	int startCol  /**< Start col of the submatrix. */,
	int endCol    /**< End col of the submatrix, or negative for counting from the end. */)
		: mrMatrix (m)
{
	// Adjust reverse addressing shorthand
	if (endRow < 0)
		endRow = m.rows + endRow;
	if (endCol < 0)
		endCol = m.cols + endCol;

	ASSERTWITH (startRow>=0 && startRow<m.rows, strformat ("Submatrix row window [%d:%d] out of bounds [0:%d].", startRow, endRow, m.rows));
	ASSERTWITH (startCol>=0 && startCol<m.cols, strformat ("Submatrix col window [%d:%d] out of bounds [0:%d].", startCol, endCol, m.cols));
	ASSERTWITH (endRow>=0 && endRow<m.rows, strformat ("Submatrix row window [%d:%d] out of bounds [0:%d].", startRow, endRow, m.rows));
	ASSERTWITH (endCol>=0 && endCol<m.cols, strformat ("Submatrix col window [%d:%d] out of bounds [0:%d].", startCol, endCol, m.cols));
	ASSERT (startRow<=endRow);
	ASSERT (startCol<=endCol);

	mStartRow = startRow;
	mStartCol = startCol;
	mEndRow   = endRow;
	mEndCol   = endCol;
	
	rows = mEndRow - mStartRow + 1;
	cols = mEndCol - mStartCol + 1;
}

double& SubMatrix::get (int row, int col)
{
	return mrMatrix.get (row + mStartRow, col + mStartCol);
}

//double& SubMatrix::get (int row, int col) const
//{
//	return mrMatrix.get (row + mStartRow, col + mStartCol);
//}


END_NAMESPACE;
