#ifndef __PACKARRAY_H__
#define __PACKARRAY_H__

#include "magic/mobject.h"
#include "magic/mmagisupp.h"

BEGIN_NAMESPACE (MagiC);

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//        ----              |     _                         /    \          //
//        |   )  ___   ___  |    / \           ___         /      \         //
//        |---   ___| |   \ | / /   \ |/\ |/\  ___| \   | <        >        //
//        |     (   | |     |/  |---| |   |   (   |  \  |  \      /         //
//        |      \__|  \__/ | \ |   | |   |    \__|   \_/   \    /          //
//                                                   \_/                    //
//////////////////////////////////////////////////////////////////////////////

/** Packed array/vector; saves space and is fast. Especially useful
 *  with the primitive types such as int or float. 0-based indexing.
 *
 *  NULL objects are not possible like in @ref Array. Nor is this
 *  container as dynamic as @ref Array is.
 **/
template <class TYPE>
class PackArray : public Object {
	TYPE*	data;		/**< The items as a C-array. */
	int		mSize;		/**< Length of the array. */
  public:

	PackArray	() {
		data = NULL;
		mSize = 0;
	}

	/** Creates an packed array with the given mSize. */
	PackArray	(int siz) {
		data = NULL;
		mSize = siz;
		make (mSize);
	}

	/** Copy constructor uses operator= to copy the contained
	 *  items. Remember to define the operator= for the object class,
	 *  as the default operator= does only a shallow copy.
	 **/
	PackArray	(const PackArray& orig) {
		data = NULL;
		make (orig.mSize);
		for (int i=0; i<mSize; i++)
			data[i] = orig.data[i];
	}

	/** Creates or recreates the array with the given mSize. */
	void	make	(int siz) {
		if (data)
			destroy ();
		mSize = siz;
		if (mSize>0)
			data = new TYPE [mSize];
		else
			data = NULL;
	}

	~PackArray	() {
		destroy ();
	}

	/** Very quick copy operator. */
	void	shallowCopy	(const PackArray& orig) {
		if (mSize != orig.mSize)
			make (orig.mSize);
		memcpy (data, orig.data, mSize*sizeof(TYPE));
	}
	
	/** Empties the array. */
	void	destroy	() {
		delete data;
		data = NULL;
		mSize = 0;
	}

	/** Empties the array. Alias for @ref destroy(). */
	void	empty () {
		destroy ();
	}

	/** Returns reference to i:th item in the array.
	 *  Const version.
	 **/
	const TYPE&	operator[]	(int i) const {
		ASSERTWITH (i>=0 && i<mSize,
					format("Index %d out of PackArray (mSize %d) bounds", i, mSize));
		return data [i];
	}

	/** Returns reference to i:th item in the array.
	 *  Non-const version.
	 **/
	TYPE&	operator[]	(int i) {
		ASSERTWITH (i>=0 && i<mSize,
					format("Index %d out of PackArray (mSize %d) bounds", i, mSize));
		return data [i];
	}

	/** Standard copy operator.
	 *
	 *  Remember to define the operator= for the object class, as the
	 *  default operator= does only a shallow copy.
	 **/
	PackArray<TYPE>& operator=	(const PackArray& other) {
		if (this != &other) {
			destroy ();
			if (other.mSize) {
				data = new TYPE [other.mSize];
				mSize = other.mSize;
				for (int i=0; i<mSize; i++)	// We have to copy these explicitly
					data[i] = other.data[i];
			}
		}
		return *this;
	}

	/*
	void	removeFill	(int pos) {
		ASSERTWITH (i>=0 && i<mSize,
					format("Index %d out of PackArray (mSize %d) bounds", i, mSize));
		delete data[pos];
		for (int i=pos; i<mSize-2; i++)
			data[i] = data[i+1];
		data[mSize-1]=NULL;
		remSize (mSize-1);
	}
	*/
	
	/** Changes mSize of the array.
	 **/
	void	resize	(int newsize) {
		ASSERT (newsize>=0);
		
		if (newsize == mSize)
			return;

		if (newsize>0)
			if (data)
				data = (TYPE*) renew (data, sizeof (TYPE)*newsize);
			else
				data = new TYPE [newsize];
		else
			destroy ();

		mSize = newsize;
	}

	int size () const {
		return mSize;
	}

	/** Implementation for @ref Object. Archive support. */
	/*
	virtual CArchive&	operator>>	(CArchive& arc) const {
		arc << mSize;
		for (int i=0; i<mSize; i++)
			arc << data[i];
		return arc;
	}
	*/
	
	/** Implementation for @ref Object. Archive support. */
	/*
	virtual IStream&	operator<<	(IStream& arc) {
		int nmSize;
		arc >> nmSize;
		destroy ();
		make (nmSize);
		for (int i=0; i<mSize; i++)
			arc >> data[i];
		return arc;
	}
	*/

	/** Implementation for @ref Object. */
	virtual void	check	() const {
		ASSERT ((data?1:0) == (mSize?1:0));
		ASSERT (((unsigned int&)*this) != 0xdddddddd);
		if (data)
			ASSERT (((unsigned int&)*data) != 0xdddddddd);
		ASSERT (mSize>=0);
		ASSERTWITH (mSize<10000000, "Reasonable array size maximum. Grow if necessary");
	}

  private:
};

END_NAMESPACE;

#endif
