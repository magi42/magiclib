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

#ifndef __MAGIC_CLASS_H__
#define __MAGIC_CLASS_H__

#include "magic/mdynamic.h"
#include "magic/mexception.h"
#include "magic/mpararr.h"
#include "magic/mrefarray.h"

BEGIN_NAMESPACE (MagiC);

// Externals
template<class K, class V> class Map;
template<class TYPE> class Array;
template<class TYPE> class RefArray;

class ClassLib;

///////////////////////////////////////////////////////////////////////////////
//                          ___  |                                           //
//                         /   \ |  ___   ____  ____                         //
//                         |     |  ___| (     (                             //
//                         |     | (   |  \__   \__                          //
//                         \___/ |  \__| ____) ____)                         //
///////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * Class of classes, contains run-time information about a class.
 *
 * Contains a number of static variables that help to build a more or
 * less complete run-time class hierarchy.
 *
 * To get into the run-time hierarchy a class must implement the
 * decl_dynamic(class) and impl_dynamic(class,parent) macros. The
 * former is given first in a class declaration in a header file, and
 * the latter is given in a source file (to create a global class
 * object).
 ******************************************************************************/
class Class /* : public Object */ {
//	decl_dynamic (Class)
	String				mName;
	Array<String>		mParentsUnfound;
	RefArray<Class>		mParents;
	//Object&			(Object::*mfpGetInstance) ();	// Instanssin luontihuukki
	GetClassInstance*	mGetInstance;
	friend class ClassLib;
  public:

	/** Empty constructor shouldn't be called ever.... */
					Class		() {FORBIDDEN}
					Class		(const char* nam, const char* bases, GetClassInstance*);
					~Class		();

	/** Adds a parent to the class with the given name. */
	void			addparent	(const char* nam) {;}

	/** Returns the name of the class. */
	const String&	getname		() const {return mName;}

	/** Prints out the class information. */
	void			printclassinfo	(FILE* out=stdout) const;

	/** Returns true if the other object is of this class. */
	bool			issameclass	(const Object& other) const {
		return (Class*)this == (Class*)(&other.getclass());
	}

	/** Dynamically creates an instance of the class. */
	Object*			getInstance	() const {return (Object*) mGetInstance->getInstance();}
};



///////////////////////////////////////////////////////////////////////////////
//                   ___  |                   |     o                        //
//                  /   \ |  ___   ____  ____ |       |                      //
//                  |     |  ___| (     (     |     | |---                   //
//                  |     | (   |  \__   \__  |     | |   )                  //
//                  \___/ |  \__| ____) ____) |____ | |__/                   //
///////////////////////////////////////////////////////////////////////////////

class ClassRef : public Object {
  public:
					ClassRef	(Class& rClass) : mpClass (&rClass) {}
	Class*			ptr			() {return mpClass;}
	const Class*	ptr			() const {return mpClass;}

  protected:
	Class* mpClass;
};

/*******************************************************************************
 * Collection of all run-time class objects in the system.
 *
 * Singular.
 *******************************************************************************/
class ClassLib : public Object {
	static Map<String,ClassRef>*	classes;
  public:
	static	void		printclassinfo	(const String& classname, FILE* out=stdout);
	static	Object&		getinstance		(const String& classname);
	static	Class*		getclass		(const String& classname);
	static	void		insert			(Class& cls);
};

END_NAMESPACE;

#endif
