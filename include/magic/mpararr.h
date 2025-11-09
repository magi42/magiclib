/***************************************************************************
    copyright            : (C) 1999 by Marko Grönroos, All Rights Reserved
    email                : magi@iki.fi
***************************************************************************/

#ifndef __PARARR_H__
#define __PARARR_H__

#include <stdlib.h>

#include <magic/mobject.h>
#include <magic/mstring.h>
#include <magic/marchive.h>
#include <magic/mmagisupp.h>

// External:

//FormatOStream& operator<< (FormatOStream& dc, int i) {;}

/*
#ifdef new
#undef new
#define new_UNDEFD
#define renew realloc
#endif
*/

BEGIN_NAMESPACE (MagiC);

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                     _                         /    \                     //
//                    / \           ___         /      \                    //
//                   /   \ |/\ |/\  ___| \   | <        >                   //
//                   |---| |   |   (   |  \  |  \      /                    //
//                   |   | |   |    \__|   \_/   \    /                     //
//                                        \_/                               //
//////////////////////////////////////////////////////////////////////////////

/** Generic array template, for objects and non-objects. Much better
 *  than manys other implementations.
 *
 *  Arrays can contain empty slots that do not point to an existing
 *  object. Some methods can also return NULL references to objects,
 *  which is rather special behaviour for C++ references. You can
 *  check the nullness of an object by the isnull(const @ref
 *  Object&)-function in mobject.h.
 *
 *  if (isnull(myArray[5])) {...}
 **/
template <class TYPE>
class Array : public Object {
	bool	mIsRef;
	int		mSize;
	TYPE**	rep;
  public:


	/** Creates an array of the given size with lower bound 0. If no size is given,
	 *  creates an empty array.
	 *
	 *  NOTE: The objects are not created with this constructor, but
	 *  only when they are accessed first time.
	 **/
	Array	(int siz=0) {
		mSize  = 0;
		rep    = NULL;
		mIsRef = false;
		make (siz);
	}
	
	Array (const Array<TYPE>& orig) {
		mSize  = 0;
		rep    = NULL;
		mIsRef = false;
		operator= (orig);
	}
	
	/** Creates an array of the given size with lower bound 0. If the array previously
	 *  has any content, that content is destroyed.
	 *
	 *  NOTE: The objects are not created with this method, but only
	 *  when they are accessed first time.
	 **/
	void	make	(int siz) {
		if (mSize) {
			empty ();
			delete rep;
			rep=NULL;
			mSize=0;
		}
		if (siz>0) {
			mSize = siz;
			ASSERTWITH (rep=new TYPE*[mSize], "Out of memory in array memory allocation");
			for (int i=0; i<mSize; i++)
				rep[i] = NULL;
		}
	}

	/** Sets a flag that tells whether is not the objects inserted in this array
	 *  are owned by the array or not. Thus, with value 'true', the class does not
	 *  delete the objects, while with 'false' it does.
	 *
	 *  The default for Array is 'false' (it owns the objects).
	 *
	 *  NOTE: This feature entirely replaces the RefArray class.
	 **/
	void	isRef	(bool isref) {mIsRef=isref;}
	
	/** Destructor destroys all the objects contained by the Array.
	 **/
	virtual	~Array () {
		empty ();
		delete rep;
		memset (this, 0xcd, sizeof(Array<TYPE>));
	}

	/** Destroys all the objects in the Array, but does NOT change the
	 *  size.
	 **/
	void	empty	() {
		if (rep)
			for (int i=0;i<mSize;i++) {
				if (!mIsRef)
					delete rep[i];
				rep[i] = NULL;
			}
	}

	/** Adds the given object to the end of the array; increments the
	 *  size of the array by one.
	 *
	 *  NOTE: Takes the ownership of the object.
	 **/
	void	add	(TYPE* i) {
		resize (mSize + 1);
		delete rep [mSize - 1];
		rep[mSize - 1] = i;
	}

	/** Adds the given object to the end of the array; increments the
	 *  size of the array by one.
	 *
	 *  NOTE: Does not takes the ownership of the object, but just
	 *  copies it using the copy constructor.
	 **/
	void	add	(const TYPE& i) {
		resize (mSize + 1);
		delete rep [mSize-1];
		if (mIsRef)
			rep[mSize-1] = const_cast<TYPE*>(&i);
		else
			rep[mSize-1] = new TYPE (i);
	}

	/** Puts the given object to the given location. Old item in the
	 *  same index location is destroyed.
	 *
	 *  NOTE: Takes the ownership of the object.
	 *
	 *  @param i The object to be inserted.
	 *  @param loc Index of the insertion point.
	 **/
	void	put	(TYPE* i, int loc) {
		ASSERTWITH (i!=NULL, "NULL-objekti put");
		if (loc<0 || loc>=mSize)
			throw out_of_range ((CONSTR)
				format("Index put %d out of Array bounds (size %d)", loc, mSize));
		if (!mIsRef)
			delete rep [loc];
		rep [loc] = i;
	}

	/** Puts the given object to the given location. Old item in the
	 *  same index location is destroyed.
	 *
	 *  NOTE: Does not takes the ownership of the object, but just
	 *  copies it using the copy constructor.
	 *
	 *  @param i The object to be inserted.
	 *  @param loc Index of the insertion point.
	 **/
	void	put	(const TYPE& i, int loc) {
		if (loc >= mSize)
			resize (loc+1);
		if (!mIsRef) {
			delete rep [loc];
			rep[loc] = new TYPE (i);
		} else
			rep[loc] = const_cast<TYPE*>(&i);
	}

	// Lisää objektin johonkin paikkaan vektoria, muutoin loppuun
	/** Adds the given object to any NULL location in the array; if no
	 *  null locations exists, the object is added to the end of the
	 *  array just like in
	 *  @ref add.
	 *
	 *  NOTE: Takes the ownership of the object.
	 **/
	void	put	(TYPE* p) {
		for (int i=0; i < mSize; i++)
			if (!rep[i]) {
				rep [i] = p;
				return;
			}
		add (p);
	}

	/** Returns a reference to the loc:th item in the Array.
	 *  If the item does not exist (is null), it is created.
	 *
	 *  Throws 'assertion_failed' if the index is out of bounds.
	 *
	 *  Const version.
	 **/
	const TYPE&	operator[]	(int loc) const {
		ASSERTWITH (loc < mSize, format("Index %d out of Array bounds (size %d)", loc, mSize));
		if (const TYPE* tmp = rep [loc])
			return *tmp;
		else if (mIsRef)
			return *((TYPE*)NULL);
		else
			return *(rep [loc] = new TYPE ());
	}

	/** Returns a reference to the loc:th item in the Array.
	 *  If the item does not exist (is null), it is created.
	 *
	 *  Throws 'assertion_failed' if the index is out of bounds.
	 *
	 *  Non-const version.
	 **/
	TYPE&	operator[]	(int loc) {
		ASSERTWITH (loc < mSize, format("Index %d out of Array bounds (size %d)", loc, mSize));
		if (TYPE* tmp = rep [loc])
			return *tmp;
		else if (mIsRef)
			return *((TYPE*)NULL);
		else
			return *(rep [loc] = new TYPE ());
	}
	
	/** Returns a pointer to the loc:th item in the Array.
	 *  If the item does not exist, a NULL is returned.
	 *
	 *  Throws 'assertion_failed' if the index is out of bounds.
	 *
	 *  Const version.
	 **/
	const TYPE*	getp	(int loc) const {
		ASSERTWITH (loc < mSize, format("Index %d out of Array bounds (size %d)", loc, mSize));
		return rep [loc];
	}

	/** Returns a pointer to the loc:th item in the Array.
	 *  If the item does not exist, a NULL is returned.
	 *
	 *  Throws 'assertion_failed' if the index is out of bounds.
	 *
	 *  Non-const version.
	 **/
	TYPE*	getp	(int loc) {
		ASSERTWITH (loc < mSize, format("Index %d out of Array bounds (size %d)", loc, mSize));
		return rep [loc];
	}

	/** Finds the given object in the Array and returns it's index
	 *  number, or -1 if not found.
	 **/
	int		find	(const TYPE& item) const {
		for (int i=0; i<mSize; i++)
			if (rep[i] == &item)
				return i;
		return -1;
	}

	/** Destroys the object with the given index number from the
	 *  array. Does not fill the hole, but leaves it NULL.
	 **/
	void	remove	(int loc) {
		ASSERTWITH (loc < mSize, format("Index %d out of Array bounds (size %d)", loc, mSize));
		delete rep[loc];
		rep[loc] = NULL;
	}

	// Poista objekti ja täytä aukko
	/** Destroys the object with the given index number from the
	 *  Array, and fills the hole by shifting the rest of the items one
	 *  index downwards.
	 **/
	void	removeFill	(int loc) {
		ASSERTWITH (loc < mSize, format("Index %d out of Array bounds (size %d)", loc, mSize));
		delete rep[loc];
		for (int i=loc+1; i<mSize; i++)
			rep[i-1]=rep[i];
		rep[mSize-1] = NULL;
		resize (mSize-1);
	}
	
	/** Removes the object with the given index from the Array, but
	 *  does NOT destruct the object; just removes the reference to
	 *  it.
	 **/
	void	cut		(int loc) {
		ASSERTWITH (loc < mSize, format("Index %d out of Array bounds (size %d)", loc, mSize));
		rep[loc] = NULL;
	}
	
	/** Changes the bounds of the Array to the given ones. New size is
	 *  calculated accordingly. Reserves or destructs as needed.
	 **/
	void	resize	(int newsize) {
		/* Muuttaa taulukon kokoa, old[lower] = new[lower] */
		if (newsize < mSize) { /* Pienennetään */
			for (int i=newsize; i<mSize; i++)
				delete rep [i];
			if (newsize==0) {
				delete rep;
				rep = NULL;
			} else
				rep = (TYPE**) renew (rep, sizeof (TYPE*)*(newsize));
		} else { /* Suurennetaan */
			if (rep)
				rep = (TYPE**) renew (rep, sizeof (TYPE*)*(newsize));
			else
				rep = (TYPE**) new TYPE* [newsize];
			for (int i=mSize; i<newsize; i++)
				rep [i] = NULL;
		}
		mSize	= newsize;
	}

	/** Standard =-operator. Performs deep copy.
	 **/
	void	operator=	(const Array<TYPE>& other) {
		empty ();
		make (other.mSize);
		for (int i=0; i<other.mSize; i++)
			if (other.rep[i])
				put (*other.rep[i], i);
	}

	/** Serialization support for the Array.
	 **/
	/* Currently not in use before I get new stream classes done.
	CArchive&	operator>>	(CArchive& arc) const {
		arc.name("lower") << lower;
		arc.name("size") << size;
		for (int i=0; i<size; i++)
			arc << ((Object&) (*this)[i]);
		return arc;
	}
	*/

	/** Serialization support for the Array.
	 **/
	/* Currently not in use before I get new stream classes done.
	IStream&	operator<<	(IStream& arc) {
		int nlower, nsize;
		arc >> nlower;
		arc >> nsize;
		resize (nlower, nlower+nsize-1);
		for (int i=0; i<size; i++) {
			arc >> ((Object&) (*this)[i]);
		}
		return arc;
	}
	*/

	/** Implementation for @ref Object. Checks the integrity of the Array.
	 **/
	virtual void	check	() const {
		ASSERT (mSize>=0);
	}

	/** Sorts the values in the Array.
	 *
	 * NOTE: The contained objects MUST inherit @ref Comparable, and
	 * be of the same class!
	 **/
	void	quicksort	() {
		qsort (rep, mSize, sizeof (TYPE*), compareComparable);
	}

	/** Returns number of elements (or null elements) in array. */
	int		size		() const {
		return mSize;
	}

	/** @ref Array iterator, although iterating with integer index may be
	 *  easier.
	 **/
	class iterator {
		const Array<TYPE>&	arr;
		int					i;
		
	  public:
						iterator				(const iterator& o) : arr (o.arr) {i=o.i;}
						iterator				(const Array<TYPE>& ar) : arr (ar) {i=0;}
		
		void			first					() {i=0;}
		void			next					() {i++;}
		int				exhausted				() const {return i>=arr.size();}
		TYPE&			getvaluev				() {return const_cast<TYPE&>(arr[i]);}
		const TYPE&		getvalue				() const {return const_cast<const TYPE&> (arr[i]);}
						operator const TYPE&	() const {return const_cast<const TYPE&> (arr[i]);}
	};

	iterator start () const {
		return iterator (*this);
	}
};

/** Clone operator for the @ref Array.
 *
 * Really clones. This must be separate from the Array, because it
 * uses the 'clone'-method of Object, which does not apply to
 * non-Object classes (such as the primitive types)
 **/
template <class TYPE>
Array<TYPE>*	clone	(const Array<TYPE>& orig) {
	Array<TYPE>* res = new Array<TYPE> (orig.size);
	for (int i=0; i<orig.size; i++)
		put ((TYPE*)orig[i].clone(), i);
}

/** Copies @ref Array by cloning (see @ref clone).
 *
 * Really clones. This must be separate from the Array, because it
 * uses the 'clone'-method of Object, which does not apply to
 * non-Object classes (such as the primitive types)
 **/
template <class TYPE>
void	copyClone	(Array<TYPE>& trg, const Array<TYPE>& orig) {
	trg.empty ();
	trg.make (orig.size());
	for (int i=0; i<orig.size(); i++)
		if (orig.getp(i))
			trg.put ((TYPE*)orig.getp(i)->clone(), i);
}


#define foreach_Array(valuetype,arrayname,varname) \
	for (ArrayIter<keytype,valuetype> varname (arrayname); \
		 !varname.exhausted(); varname.next())


/*
#ifdef new_UNDEFD
#define new DEBUG_NEW
#undef renew
#undef new_UNDEFD
#endif
*/

END_NAMESPACE;

#endif
