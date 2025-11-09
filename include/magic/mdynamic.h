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

#ifndef __DYNAMIC_H__
#define __DYNAMIC_H__

#ifndef BEGIN_NAMESPACE
#define BEGIN_NAMESPACE(ns) namespace ns {
#ifdef QT_VERSION
#define END_NAMESPACE }
#else
#define END_NAMESPACE } using namespace MagiC;
#endif
#endif

BEGIN_NAMESPACE(MagiC);

class Object;

/*******************************************************************************
 * Callback class for retrieving a new instance of the class
 ******************************************************************************/
class GetClassInstance {
  public:
	virtual 			~GetClassInstance () {}
    virtual Object*		getInstance	()=0;
};

#define CLONEMETHOD(cls) cls* clone () const {return new cls (*this);}
#define decl_clonable(classname) public: CLONEMETHOD (classname) private:

/*******************************************************************************
 * Declares the dynamic features of a class
 *
 * This macro should be used in class declaration of a dynamic class.
 ******************************************************************************/
#define decl_dynamic(classname) \
	static Class* class_##classname;\
  public:\
	virtual	const Class&	getclass	() const {return *class_##classname;}\
  protected:

//    virtual Object*		getinstance	();

/*******************************************************************************
 * Implements the dynamic features of a class
 *
 * This macro should be used in the definition file of a dynamic class.
 ******************************************************************************/
#define impl_dynamic(classname,bases) \
	class GetInstance_##classname : public GetClassInstance {\
		virtual 			~GetInstance_##classname () {}				\
	  public: Object* getInstance(){return new classname();}	\
	};\
	Class*	classname::class_##classname = new Class (#classname, #bases, new GetInstance_##classname());\

/**
 * Implements dynamic features for an inner class.
 **/
#define impl_inner_dynamic(outerclass,classname,bases)			\
	class GetInstance_##classname : public GetClassInstance {\
		virtual 			~GetInstance_##classname () {}				\
	  public: Object* getInstance(){return new outerclass::classname();} \
	};\
	Class*	outerclass::classname::class_##classname = new Class (#classname, #bases, new GetInstance_##classname()); \

//    Object* classname::getinstance () {return new classname();}
//	Class	classname::class_##classname (#classname, #bases, (Object& (Object::*)()) (&classname::getinstance));
//	Class	classname::class_##classname (#classname, #bases, (const Object& (Object::*)()) (&classname::getinstance));

/*******************************************************************************
 * Implements the dynamic features of an abstract class
 ******************************************************************************/
#define impl_abstract(classname,bases) \
	Class*	classname::class_##classname = new Class (#classname, #bases, NULL);\

//    Object* classname::getinstance () {return NULL;}
//    Object& classname::getinstance () {return (Object&) *NULL;}
//	Class	classname::class_##classname (#classname, #bases, (Object& (Object::*)()) (&classname::getinstance));
//	Class	classname::class_##classname (#classname, #bases, (const Object& (Object::*)() const) (&classname::getinstance));

/*******************************************************************************
 * Creates an object of the given class dynamically
 ******************************************************************************/
#define newobj(classname) \
	((classname*) & ClassLib::getinstance(#classname))

/*******************************************************************************
 * Creates an object of the given class dynamically
 ******************************************************************************/
#define dyncreate(classname) \
	(&ClassLib::getinstance(classname))

//#define getInstance(classname) dyncreate(classname)


END_NAMESPACE;

#endif
