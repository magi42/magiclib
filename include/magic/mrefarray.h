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

#ifndef __REFARRAY_H__
#define __REFARRAY_H__

BEGIN_NAMESPACE (MagiC);


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//           ----              _                         /    \             //
//           |   )  ___   __  / \           ___         /      \            //
//           |---  /   ) /   /   \ |/\ |/\  ___| \   | <        >           //
//           | \   |---  +-- |---| |   |   (   |  \  |  \      /            //
//           |  \   \__  |   |   | |   |    \__|   \_/   \    /             //
//                       |                        \_/                       //
//////////////////////////////////////////////////////////////////////////////

template <class TYPE>
// Sisältää viittauksia, ei oikeita objekteja. Tämän merkitys on lähinnä
// siinä, ettei objekteja destruktoida vektorin tuhoutuessa
class RefArray {
	TYPE**	mpRep;
	int		mSize;
  public:

	RefArray	() {
		mSize	= 0,
		mpRep	= NULL;
	}

	// Create the reference array to refer to a real container array
	RefArray	(const Array<TYPE>& orig) {
		mSize	= 0;
		mpRep	= NULL;
		*this   = orig;
	}

	// Copy constructor
	RefArray	(const RefArray<TYPE>& orig) {
		mSize	= 0;
		mpRep	= NULL;
		*this   = orig;
	}
	
	RefArray	(int size) {
		mSize	= 0;
		mpRep	= NULL;
		make (size);
	}

	void	make	(int size) {
		delete mpRep;
		mpRep	= new TYPE* [size];
		mSize	= size;

		for (int i=0; i<size; i++)
			mpRep [i] = NULL;
	}

	int		size	() const {return mSize;}

	void	operator=	(const Array<TYPE>& orig) {
		make (orig.size());
		for (int i=0; i<mSize; i++)
			mpRep[i] = (TYPE*) orig.getp (i);
	}

	void	operator=	(const RefArray<TYPE>& orig) {
		make (orig.size());
		for (int i=0; i<mSize; i++)
			mpRep [i] = orig.mpRep[i];
	}

	virtual	~RefArray () {
		delete mpRep;
	}

	/** Empties the array from all items. */
	void	empty	() {
		delete mpRep;
		mpRep = NULL;
		mSize = 0;
	}

	/** Adds a new reference item to the array. */
	void	add	(TYPE* pItem) {
		resize (mSize + 1);
		mpRep[mSize-1] = pItem;
	}

	/** Puts a new reference item to a specific position in the array. */
	void	put	(TYPE* pItem, int loc) {
		PRE (pItem != NULL && loc >= 0 && loc < mSize);
		delete mpRep [loc];
		mpRep [loc] = pItem;
	}

	// Palauttaa viittauksen taulukon haluttuun alkioon
	TYPE&	operator[]	(int loc) {
		PRE (loc >= 0 && loc < mSize);
		return *mpRep [loc];
	}

	// Edellisen const-versio
	const TYPE&	operator[]	(int loc) const {
		PRE (loc >= 0 && loc < mSize);
		return *mpRep [loc];
	}

	void	resize	(int newsize) {
		/* Muuttaa taulukon kokoa, old[lower] = new[lower] */
		PRE (newsize > 0);
		if (newsize < mSize) { /* Pienennetään */
			mpRep = (TYPE**) renew (mpRep, sizeof (TYPE*)*(newsize));
		} else { /* Suurennetaan */
			mpRep = (TYPE**) renew (mpRep, sizeof (TYPE*)*(newsize));
			for (int i=mSize; i < newsize; i++)
				mpRep[i] = NULL;
		}
		mSize = newsize;
	}

	/** Sorts the references... WOW!
	 *
	 * The contained class MUST inherit Comparable...
	 **/
	void	quicksort	() {
		qsort (mpRep, mSize, sizeof (TYPE*), compareComparable);
	}
};

#define CRefArray RefArray

END_NAMESPACE;

#endif
