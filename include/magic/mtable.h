/***************************************************************************
 *   This file is part of the MagiC++ library.                             *
 *                                                                         *
 *   Copyright (C) 1998-2004 Marko Grönroos <magi@iki.fi>                  *
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

#ifndef __TABLE_H__
#define __TABLE_H__

#include<magic/mobject.h>
#include<magic/mstring.h>
#include<magic/mexception.h>

BEGIN_NAMESPACE (MagiC);

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//        ----              |   -----             |         /    \          //
//        |   )  ___   ___  |     |    ___  |     |  ___   /      \         //
//        |---   ___| |   \ | /   |    ___| |---  | /   ) <        >        //
//        |     (   | |     |/    |   (   | |   ) | |---   \      /         //
//        |      \__|  \__/ | \   |    \__| |__/  |  \__    \    /          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

// Obey the global no-bounds-checking option
#ifndef NOCHECKBOUNDS
#define PACKTABLE_CHECKBOUNDS 1
#endif


/** Packed two-dimensional table; works also with native types.
 *
 *  Ideal for storing numeric matrices; any mathematical matrix
 *  implementation would most probably inherit PackTable<double>.
 **/
template <class TYPE>
class PackTable : public Object {
  protected:
	TYPE*	mData;
  public:
	/** Creates a null-sized table. Dimensions can be given later with
	 *  the @ref make() method.
	 **/
	PackTable	() {
		mData = NULL;
		rows   = 0,
		cols   = 0;
	}

	/** Creates a Table.
	 *
	 *  @param rs Number of rows in the Table.
	 *  @param cs Number of columns in the Table.
	 **/
	PackTable	(int rs, int cs) {
		mData = NULL;
		rows   = 0,
		cols   = 0;
		make (rs, cs);
	}

	/** Copy constructor.
	 *
	 *  Copying is shallow by default: works *fast* with native types,
	 *  but messes more complex objects up. Use with caution.
	 *
	 *  @param orig Original table to be copied.
	 *  @param deep Should we copy deep?
	 **/
	PackTable	(const PackTable& orig, bool deep=false) {
		mData = NULL;
		rows   = cols = 0;
		if (orig.mData) {
			if (deep) {
				// Deep copy requested
				make (orig.rows, orig.cols);
				for (int i=0; i<rows; i++)
					for (int j=0; j<cols; j++)
						get(i,j) = orig.get(i,j);
			} else {
				// Default fast but shallow copy
				rows = orig.rows;
				cols = orig.cols;
				mData = new TYPE[rows*cols];
				memcpy (mData, orig.mData, rows*cols*sizeof(TYPE));
			}
		}
	}
	
	~PackTable	() {
		destroy ();
	}

	/** Destroys the content of the Table, as well as its
	 *  dimensions. Can be resized later with the @ref make() method.
	 **/
	void	destroy	() {
		delete mData;
		mData = NULL;
		rows   = 0,
		cols   = 0;
	}

	/** Creates or recreates the Table with the given dimensions. If
	 *  the Table has any old contents, they are destroyed.
	 *
	 *  @param nrows Number of rows in the Table.
	 *  @param ncols Number of columns in the Table.
	 **/
	virtual void	make	(int nrows, int ncols) {
		ASSERT (nrows>=0 && ncols>=0);
		if (mData)
			destroy ();
		rows = nrows,
		cols = ncols;
		if (rows>0 && cols>0) {
			// Luodaan uusi tietorakenne valmiiksi
			newComment (format ("PackTable<> data, %d rows, %d cols", rows, cols));
			mData = new TYPE [rows*cols];
		}
	}

	/** Retrieves a reference to an item in the Table. Const
	 *  version.
	 *
	 *  @param row Row index of the item. 0-based.
	 *  @param col Column index of the item. 0-based.
	 **/
	virtual const TYPE&	get	(int row, int col) const {
#ifdef PACKTABLE_CHECKBOUNDS
		if (!(row>=0 && row<rows && col>=0 && col<cols))
			throw out_of_bounds (strformat ("Table range overflow: %d,%d out of %d,%d",
											row, col, rows, cols));
#endif
		return mData[row*cols+col];
	}

	/** Retrieves a reference to an item in the Table. Non-const
	 *  version.
	 *
	 *  @param row Row index of the item. 0-based.
	 *  @param col Column index of the item. 0-based.
	 **/
	virtual TYPE&	get	(int row, int col) {
#ifdef PACKTABLE_CHECKBOUNDS
		if (!(row>=0 && row<rows && col>=0 && col<cols))
			throw out_of_bounds (strformat ("Table range overflow: %d,%d out of %d,%d",
											row, col, rows, cols));
#endif
		return mData[row*cols+col];
	}

	/** Serialization implementation for @ref Object.
	 **/
	/*
	virtual CArchive&	operator>>	(CArchive& arc) const {
		arc << rows << cols;
		for (int i=0; i<rows; i++)
			for (int j=0; j<cols; j++)
				arc << get (i, j);
		return arc;
	}
	*/

	/** Serialization implementation for @ref Object.
	 **/
	/*
	IStream&	operator<<	(IStream& arc) {
		int nrows, ncols;
		arc >> nrows >> ncols;
		make (nrows, ncols);
		for (int i=0; i<rows; i++)
			for (int j=0; j<cols; j++)
				arc >> get (i, j);
		return arc;
	}
	*/

	/** Number of rows in the table. */
	int	rows;

	/** Number of columns in the table. */
	int	cols;
	
  private:
	void operator=	(const PackTable& other) {} /**< Not allowed. */
};

END_NAMESPACE;

#endif
