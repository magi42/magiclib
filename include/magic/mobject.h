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

#ifndef __MAGIC_OBJECT_H__
#define __MAGIC_OBJECT_H__

#include <stdio.h>
#include <iostream>
#include <typeinfo>			// Used by dynamic_cast

#include "magic/mdynamic.h"
#include "magic/mdebug.h"

#ifndef WORD
#if __WORDSIZE == 64
#define WORD long
#else
#define WORD int
#endif
#endif


/////////////////////////////////////////////////////////////////////////////
// Definitions
/////////////////////////////////////////////////////////////////////////////

#ifndef BEGIN_NAMESPACE
#define BEGIN_NAMESPACE(ns) namespace ns {
#ifdef QT_VERSION
#define END_NAMESPACE }
#else
#define END_NAMESPACE } using namespace MagiC;
#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// Predeclarations
/////////////////////////////////////////////////////////////////////////////

namespace MagiC {
    // Internal predeclarations
	class Object;
	class Comparable;
	
	// External predeclarations
	class Class;
    class OStream;
	class TextOStream;
	class TextIStream;
	class DataOStream;
	class DataIStream;
	template <class TYPE> class Array;
	
	class String;
}

// using namespace MagiC;
using namespace std;
BEGIN_NAMESPACE (MagiC);

/////////////////////////////////////////////////////////////////////////////


/** Returns 1 (true) if the given object reference is NULL, 0 if not.
 *  Yes, it's not strictly "good C++" to handle references like they
 *  were pointers. But, useful nevertheless.
 **/
inline int isnull (const Object& obj) {return ! (&obj);}

#define isnullref(ref) ((&ref)==NULL)

ostream&     operator<< (ostream& out, const Object& obj);
istream&     operator>> (istream& in, Object& obj);
OStream&     operator<< (OStream& out, const Object& obj);
TextOStream& operator<< (TextOStream& out, const Object& obj);
TextIStream& operator>> (TextIStream& in, Object& obj);
DataOStream& operator<< (DataOStream& out, const Object& obj);
DataIStream& operator>> (DataIStream& in, Object& obj);



///////////////////////////////////////////////////////////////////////////////
//                       ___          o                                      //
//                      |   | |          ___   ___   |                       //
//                      |   | |---    | /   ) |   \ -+-                      //
//                      |   | |   )   | |---  |      |                       //
//                      `___´ |__/  \_|  \__   \__/   \                      //
///////////////////////////////////////////////////////////////////////////////

class Object {
	decl_dynamic (Object);

  public:
							Object			() {mRefCount = 0;}
	virtual					~Object			();
	virtual	ostream&		operator>>		(ostream&) const;
	virtual	istream&		operator<<		(istream&);
	virtual	OStream&		operator>>		(OStream&) const;
	virtual	TextOStream&	operator>>		(TextOStream&) const;
	virtual	TextIStream&	operator<<		(TextIStream&);
	virtual	DataOStream&	operator>>		(DataOStream&) const;
	virtual	DataIStream&	operator<<		(DataIStream&);
	virtual	Object*			clone			() const;
	virtual void			check			() const;
	bool					isOK			() const;
	const String&			getclassname	() const;
	int						is_a			(const String& classname) const;
	inline void				incRef			() {mRefCount++;} /**< Increments the reference counter of the object. */
	inline int				decRef			() {mRefCount--; return mRefCount;} /**< Decrements the reference counter of the object. */
	inline int				refCount		() const {return mRefCount;} /**< Returns the number of references to the object. */
	
#ifdef DEBUG_OBJECT_NEW	
#ifdef new
#undef new
	void*					operator new[]	(size_t size, const char* filen, int lineno, const char* funcn);
	void*					operator new	(size_t size, const char* filen, int lineno, const char* funcn);
	void*					operator new[]	(size_t size);
	void*					operator new	(size_t size);
#define new DEBUG_NEW
#endif
#endif

  private:
	int		mRefCount;
};

extern const char nullchar;



///////////////////////////////////////////////////////////////////////////////
//          ----              /  ___          o                 \            //
//          |   )  ___   __  /  |   | |          ___   ___   |   \           //
//          |---  /   ) /   <   |   | |---    | /   ) |   \ -+-   >          //
//          | \   |---  +--  \  |   | |   )   | |---  |      |   /           //
//          |  \   \__  |     \ `___´ |__/  \_|  \__   \__/   \ /            //
//                      |                                                    //
///////////////////////////////////////////////////////////////////////////////

template <class TYPE>
class Ref {
  public:
				Ref						(TYPE* object=NULL)		{mpObject = object; if (mpObject) mpObject->incRef();}
				Ref						(const Ref<TYPE>& ref)	{mpObject = ref.mpObject; if (mpObject) mpObject->incRef();}
				~Ref					()						{if (mpObject && mpObject->decRef() == 0) delete mpObject;}

	TYPE&		object					()						{return *mpObject;}
	const TYPE&	object					() const				{return *mpObject;}
				operator TYPE&			()						{return *mpObject;}
				operator const TYPE&	() const				{return *mpObject;}
	TYPE*		operator ->				()						{return mpObject;}
	const TYPE*	operator ->				() const				{return mpObject;}
	TYPE&       operator =              (TYPE* object)			{if (mpObject && mpObject->decRef() == 0) delete mpObject; mpObject = object; if (mpObject) mpObject->incRef();}
	TYPE&       operator =              (const Ref<TYPE>& ref)	{if (mpObject && mpObject->decRef() == 0) delete mpObject; mpObject = ref.mpObject; if (mpObject) mpObject->incRef();}
	bool		isNull					() const				{return mpObject==NULL;}
	TYPE*       getReleasedPtr          ()						{TYPE* tmp=mpObject; mpObject->decRef(); mpObject = NULL; return tmp;}

  private:
	TYPE*	mpObject;
};



///////////////////////////////////////////////////////////////////////////////
//             ___                                        |                  //
//            /   \             --   ___       ___  |     |  ___             //
//            |      __  |/|/| |  )  ___| |/\  ___| |---  | /   )            //
//            |     /  \ | | | |--  (   | |   (   | |   ) | |---             //
//            \___/ \__/ | | | |     \__| |    \__| |__/  |  \__             //
///////////////////////////////////////////////////////////////////////////////

/** An @ref Object that can be compared, for sorting purposes, etc. It
 *  has also a hash function for hashing.
 **/
class Comparable : public Object {
	decl_dynamic (Comparable);
  public:
					Comparable	() {;}

	/** Hash function. Inheritors should overload this if they want to
	 *  be used as a key in @ref Map, etc.
	 *
	 *  @return Hash value.
	 *
	 *  @param hashsize Hash size.
	 **/
	virtual int		hashfunc	(int hashsize) const {return 0;}

	/** Equality comparison. Must be overloaded. */
	virtual int		operator==	(const Comparable& other) const=0;

	/** This should be overloaded to enable quicksorting of the
	 *  inheritor.
	 **/
	virtual int		compare		(const Comparable& other) const {return 0;}
};

/** Callback function for running qsort... */
extern int compareComparable	(const void* a, const void* b);



///////////////////////////////////////////////////////////////////////////////
//                               ---                                         //
//                                |    _    |                                //
//                                |  |/ \  -+-                               //
//                                |  |   |  |                                //
//                               _|_ |   |   \                               //
///////////////////////////////////////////////////////////////////////////////

/** Object-integer.
 **/
class Int : public Comparable {
	decl_dynamic (Int)
	long	data;
  public:

				Int				()								{data=0;}
				Int				(const Int& o)					{data=o.data;}
	explicit	Int				(const String& o);
	explicit	Int				(int o)							{data=o;}
	explicit	Int				(long o)						{data=o;}
	int			toInt			() const						{return data;}
	long		toLong			() const						{return data;}
	long&		toLongRef		()								{return data;}
	int			operator <		(const Int& o)					{return data<o.data;}
	int			operator ==		(const Int& o)					{return data==o.data;}
	int			hashfunc		(int hashsize) const			{return data % hashsize;}
	int			operator ==		(const Comparable& o) const;
	int			operator !=		(const Int& o)					{return data!=o.data;}
	Object*		clone			() const						{return new Int (*this);}
};



//////////////////////////////////////////////////////////////////////////////
//                          ----- |                                         //
//                          |     |       ___   |                           //
//                          |---  |  __   ___| -+-                          //
//                          |     | /  \ (   |  |                           //
//                          |     | \__/  \__|   \                          //
//////////////////////////////////////////////////////////////////////////////

/** Object-floating poing value.
 **/
class Float : public Comparable {
	double	mValue;
  public:

				Float			()								: mValue (0) {}
				Float			(float f)						: mValue (f) {}
				Float			(const Float& o)				: mValue (o.mValue) {}
	int			operator ==		(const Comparable& o) const		{return 0;}
	Object*		clone			() const						{return new Float (*this);}
				operator double	() const						{return mValue;}
};



//////////////////////////////////////////////////////////////////////////////
//                 ___   ___  |   |          |       |                      //
//                /   \ /   \ |\ /|          |       |  ___                 //
//                |     |     | V |  __   ---| |   | | /   )                //
//                |     |     | | | /  \ (   | |   | | |---                 //
//                \___/ \___/ |   | \__/  ---|  \__! |  \__                 //
//////////////////////////////////////////////////////////////////////////////

/** A module. Some object files may want to register this if they want
 *  to use tracking of how the object files are loaded.
 **/
class CPPModule {
	char* name;
  public:
				CPPModule	(const char* n);
				~CPPModule	() {delete name;}
};

#define decl_module(modname) CPPModule mod_##modname (#modname);


// Ok, include this here now when the Object has been well defined
// #include "magic/mclass.h"


END_NAMESPACE;

///////////////////////////////////////////////////////////////////////////////
// Now we can include these default includes

// Required by magisupp
#include "magic/mstring.h"

// All exception and assertion handling, etc.
#include "magic/mmagisupp.h"
#endif

//


