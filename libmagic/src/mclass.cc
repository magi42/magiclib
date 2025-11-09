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

#include "magic/mclass.h"
#include "magic/mpararr.h"
#include "magic/mrefarray.h"
#include "magic/mmap.h"
#include "magic/mdatastream.h"
#include "magic/mtextstream.h"

BEGIN_NAMESPACE (MagiC);


///////////////////////////////////////////////////////////////////////////////
//                          ___  |                                           //
//                         /   \ |  ___   ____  ____                         //
//                         |     |  ___| (     (                             //
//                         |     | (   |  \__   \__                          //
//                         \___/ |  \__| ____) ____)                         //
///////////////////////////////////////////////////////////////////////////////

//decl_dynamic (Class)

Class*	appclass = NULL;
Class*	taskclass = NULL;

//Class::Class (const char* nam, const char* bases, Object& (Object::*getinst)()){
Class::Class (const char* nam, const char* bases, GetClassInstance* getInstPtr){
	#ifdef MODULEDEBUG
	TRACE2 ("Class (%s, %s)", nam, bases);
	#endif
	mName = nam;
	mName.checksum ();

#ifdef DEBUG_CLASS_CREATION
	if (nam)
		fprintf (stderr, "<<<Class %s (%s) created>>>\n", nam, bases);
	else
		fprintf (stderr, "<<<UNNAMED CLASS CREATED>>>\n");
#endif
	
	// mParentsUnfound = new Array<String>;
	// mpParents = new RefArray<Class>;
	//	getinstance = (Object&(Object::*getinst)()) ();
	//getinstance = (Object&(Object::*)()) getinst;
	mGetInstance = getInstPtr;

	if (bases) {
		// Remove extra characters {""} from ends of the parent class list
		String bases_str = bases;
		while (bases_str.length()>0 && (bases_str[0]=='{' || bases_str[0]=='"'))
			bases_str = bases_str.mid (1);
		while (bases_str.length()>0 && (bases_str[bases_str.length()-1]=='}' || bases_str[bases_str.length()-1]=='"'))
			bases_str.dellast (1);

		// Go through the list of parent classes
		bases_str.split (mParentsUnfound, ',');
		for (int i=0; i<mParentsUnfound.size(); i++) {
			if ((mParentsUnfound[i] == "Application" ||
				 mParentsUnfound[i] == "HTMLQuery") &&
				mName != "HTMLQuery") {
				appclass = this;
				// TRACE2 ("Class (%s, %s)", nam, bases);
			}
			if (mParentsUnfound[i] == "Task")
				taskclass = this;
		}
	}

	// printclassinfo ();
	
	ClassLib::insert (*this);
}

Class::~Class () {
	//delete mpName;
	//delete mpParentsUnfound;
	//delete mpParents;
}

void Class::printclassinfo (FILE* out) const {
	fprintf (out, "class %s",
			 (CONSTR) mName);

	// Tulostetaan vanhemmat
	if (mParentsUnfound.size() > 0) {
		fprintf (out, ": ");
		for (int i=0; i<mParentsUnfound.size(); i++) {
			fprintf (out, "%s", (CONSTR) mParentsUnfound[i]);
			if (i < mParentsUnfound.size() - 1)
				fprintf (out, ",");
		}
	}
	fprintf (out, " {\n");

	// Tulostetaan attribuutit

	// NOT WORKING TEMPORARILY!
	//Object* inst = (const_cast<Class*>(this)->getinstance ());
	Object* inst=NULL;

	DataOStream ar (out);
	ar << *inst;
	for (int i=0; i<ar.getattribs().size(); i++)
		fprintf (out, "\t%s;\n", (CONSTR) ar.getattribs()[i]);
	delete inst;
	fprintf (out, "};\n");
}



///////////////////////////////////////////////////////////////////////////////
//                   ___  |                   |     o                        //
//                  /   \ |  ___   ____  ____ |       |                      //
//                  |     |  ___| (     (     |     | |---                   //
//                  |     | (   |  \__   \__  |     | |   )                  //
//                  \___/ |  \__| ____) ____) |____ | |__/                   //
///////////////////////////////////////////////////////////////////////////////

// Class* ClassLib::firstclass = NULL;
CMap<String,ClassRef>* ClassLib::classes = NULL;

void ClassLib::printclassinfo (const String& classname, FILE* out) {
	const Class* cls = classes->getp (classname)->ptr ();
	if (cls)
		cls->printclassinfo (out);
}

Class* ClassLib::getclass (const String& classname) {
	return classes->getvp (classname)->ptr();
}

Object& ClassLib::getinstance (const String& classname) {
	//TRACE1("%s", (CONSTR) classname);
	Class* cls = classes->getvp (classname)->ptr ();
	if (cls)
		return *cls->getInstance();
	TRACE1("%%ERROR: No instance for '%s' found", (CONSTR) classname);
	return *(Object*)NULL;
}

void ClassLib::insert (Class& cls) {
	if (!classes)
		classes = new CMap<String,ClassRef> (256, MAP_REF);
	
	classes->set (cls.mName, new ClassRef (cls));

	// Haetaan luokan vanhemmat
	for (int i=0; i<cls.mParentsUnfound.size(); i++)
		;
}

END_NAMESPACE;
