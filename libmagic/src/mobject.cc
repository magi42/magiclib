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

#include <iostream>
#include <malloc.h>
#include <stdlib.h>

#include "magic/mobject.h"
#include "magic/mclass.h"
#include "magic/mmath.h"
#include "magic/mmagisupp.h"
#include "magic/mdatastream.h"
#include "magic/mpararr.h"

BEGIN_NAMESPACE (MagiC);

decl_module (object);

//- Standard object.hierarchy<BR>
//- Käytetty seuraavissa käyttöympäristöissä, vaikkakaan yhteensopivuus
//- ei välttämättä ole täysin ajan tasalla:
//-		 MSDOS Borland C++ 3.0
//-		 Unix gcc 2.7.0 C++ (Solaris, Linux)
//-		 Unix egcs C++ (Solaris, Linux)


const char nullchar = '\x00';
FILE* dbgout = stderr;

impl_abstract (Object, {})
impl_abstract (Comparable, {Object});
impl_dynamic (Int, {Comparable})

/** Outputs any @ref Object to the standard C++ stream, if the operator<< has been
 *  defined for it.
 *
 *  For example:
 *
 *  String myString = "Hello, World!";
 *
 *  cout << myString << "\n";
 **/
ostream& operator<< (ostream& out, const Object& obj) {
	obj >> out;
	return out;
}

istream& operator>> (istream& in, Object& obj) {
	obj << in;
	return in;
}

///////////////////////////////////////////////////////////////////////////////////////////
//                                                                                       //
//  ----                                            | o                     o            //
// (      |       ___   ___                ___      |        ___   ___   |           _   //
//  ---  -+- |/\ /   )  ___| |/|/|    |/\ /   )  ---| | |/\ /   ) |   \ -+- |  __  |/ \  //
//     )  |  |   |---  (   | | | |    |   |---  (   | | |   |---  |      |  | /  \ |   | //
// ___/    \ |    \__   \__| | | |    |    \__   ---| | |    \__   \__/   \ | \__/ |   | //
//                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////

OStream& operator<< (OStream& out, const Object& obj)
{
	obj >> out;
	return out;
}

/** Outputs any @ref Object to the MagiClib stream, if the operator<<
 *  has been defined for it.
 *
 *  For example:
 *
 *  String myString = "Hello, World!";
 *
 *  sout << myString << "\n";
 **/
TextOStream& operator<< (TextOStream& out, const Object& obj)
{
	obj >> out;
	return out;
}

/** Reads any @ref Object from the standard C++ stream, if the
 *  operator>> has been defined for it.
 **/
TextIStream& operator>> (TextIStream& in, Object& obj)
{
	obj << in;
	return in;
}

/** Serialize any @ref Object to the MagiClib archive, if the operator<<
 *  has been defined for it.
 *
 *  NOTE: The arciving operators have been working fine in past, but
 *  have not been tested for a long time now. They might not work.
 **/
DataOStream& operator<< (DataOStream& out, const Object& obj)
{
	if (out.mFormatMode & DataOStream::FMT_TEXT)
		out.printComma ();

	out.mDepth++; // Grow indentation depth

	if (out.mFormatMode & DataOStream::FMT_RECORD) {
		// With FMT_RECORD, we do not actually print anything,
		// but just store the attributes.
		
		if (out.mDepth==1)
			obj >> out;
		else
			out.mAttribs->add (new String (out.nextName()));
	} else {
		if (out.mFormatMode & DataOStream::FMT_TEXT) {
			if ((out.mFormatMode & DataOStream::FMT_CLASSNAMES) && !isnull (obj.getclass().getname()))
				out << obj.getclass().getname();
			//		, out.print ("::");

			if (!out.mNextName.isEmpty()) {
				out << out.nextName();
				out << '=';
				out.name ("");  // NULL originally, but causes ambiquity with some compilers
			}
			out << '{';
		}

		// Let the object output its members, recursively.
		obj >> out;

		if (out.mFormatMode & DataOStream::FMT_TEXT)
			out << '}';
	}
	
	out.mDepth--; // Lesser indentation depth

	return out;
}

/** Reads any @ref Object from a MagiClib archive, if the operator>>
 *  has been defined for it.
 *
 *  NOTE: The arciving operators have been working fine in past, but
 *  have not been tested for a long time now. They might not work.
 **/
DataIStream& operator>> (DataIStream& in, Object& obj) {
	// in.depth++;
	obj << in;
	// in.depth--;
	return in;
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                       ___          o                                      //
//                      |   | |          ___   ___   |                       //
//                      |   | |---    | /   ) |   \ -+-                      //
//                      |   | |   )   | |---  |      |                       //
//                      `___´ |__/  \_|  \__   \__/   \                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/** @class Object
 *  @brief Highest abstract baseclass.
 *
 *  Includes internal reference counter for all objects, for using the
 *  Ref<> references.
 *
 *  NOTE: Not actually abstract because of the RTTI system we use.
 **/

Object::~Object () {
}

/** Output to standard C++ streams. */
ostream& Object::operator>> (ostream& out) const
{
	return out;
}

/** Input from standard C++ streams. */
istream& Object::operator<< (istream& in)
{
	return in;
}

OStream& Object::operator>> (OStream& out) const
{
	return out;
}
		
/** Serializes the object into a data stream.
 *
 *  To save objects into file, you should reimplement this.
 **/
DataOStream& Object::operator>> (DataOStream& out) const
{
	return out;
}

/** Unserializes the object from a data stream.
 *
 *  You can use this to load serialized objects from file.
 **/
DataIStream& Object::operator<< (DataIStream& in)
{
	MUST_OVERLOAD;
	return in;
}

/** Prints the object into text stream.
 **/
TextOStream& Object::operator>> (TextOStream& out) const
{
	return out;
}

/** Unserializes the object from a text stream.
 *
 *  You can use this to load serialized objects from file.
 **/
TextIStream& Object::operator<< (TextIStream& in)
{
	MUST_OVERLOAD;
	return in;
}

/** Clone should perform a full recursive cloning of the object.
 *  The return type should vary by implementor.
 **/
Object* Object::clone () const {
	MUST_OVERLOAD;
	return NULL;
}

/** Checks the integrity of the object, throws check_failed
 *  exception if fails.
 **/
void Object::check () const
{
}

/** Sugar for the
 *  @ref check method. Catches the exception and returns a boolean value.
 **/
bool Object::isOK () const {
	try {
		check ();
	} catch (...) {
		return false;
	}
	return true;
}

/** Returns the classname if the class has RTTI information
 *  defined with the decl_dynamic macro.
 **/
const String& Object::getclassname () const
{
	return getclass().getname ();
}

/** Returns true of the object is of the given class. The method
 *  does not currently check inheritance, as it should.
 **/
int Object::is_a (const String& classname) const
{
	return getclass().getname () == classname;
}

bool nextNewIsObject = false;

#ifdef DEBUG_OBJECT_NEW
#ifdef new
#undef new

/** New operator that provides all kinds of nice
 *  information. Defined only if DEBUG_OBJECT_NEW has been defined
 *  in mdebug.h. The new operator is macro-redefined to use this.
 *
 *  @see mdebug.h
 *
 *  @param size Size of the allocated block. This is added by the C++ allocation system.
 *  @param filen File name of the allocation request.
 *  @param lineno Line number where the allocation was made.
 *  @param funcn Function name where the allocation was made.
 **/
void* Object::operator new[] (size_t size, const char* filen, int lineno, const char* funcn) {
	nextNewIsObject = true;
	return ::operator new[] (size, filen, lineno, funcn);
}

/** New operator that provides all kinds of nice
 *  information. Defined only if DEBUG_OBJECT_NEW has been defined
 *  in mdebug.h. The new operator is macro-redefined  to use this.
 *
 *  @see mdebug.h
 *
 *  @param size Size of the allocated block. This is added by the C++ allocation system.
 *  @param filen File name of the allocation request.
 *  @param lineno Line number where the allocation was made.
 *  @param funcn Function name where the allocation was made.
 **/
void* Object::operator new (size_t size, const char* filen, int lineno, const char* funcn) {
	nextNewIsObject = true;
	return ::operator new[] (size, filen, lineno, funcn);
}

/** New operator that provides all kinds of nice
 *  information. Defined only if DEBUG_OBJECT_NEW has been defined
 *  in mdebug.h.
 **/
void* Object::operator new[] (size_t size) {
	nextNewIsObject = true;
	return ::operator new[] (size, "unknown", -1, "unknown");
}

/** New operator that provides all kinds of nice
 *  information. Defined only if DEBUG_OBJECT_NEW has been defined
 *  in mdebug.h.
 **/
void* Object::operator new (size_t size) {
	nextNewIsObject = true;
	return ::operator new[] (size, "unknown", -1, "unknown");
}

#define new DEBUG_NEW
#endif
#endif


///////////////////////////////// int ///////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////

int compareComparable	(const void* a, const void* b) {
	/*
	  // :WEIRDNESS: The code below crashes... What's the difference
	  ASSERT (a);
	  ASSERT (b);
	  const Comparable& ac = *static_cast<const Comparable*> (a);
	  const Comparable& bc = *static_cast<const Comparable*> (b);
	  return ac.compare (bc);
	  */

	return (**(const Comparable**)a).compare ((**(const Comparable**)b));
}
/*
int compareComparable	(const void* a, const void* b) {
	return (**(const Comparable**)a).compare ((**(const Comparable**)b));
}
*/

///////////////////////////////////////////////////////////////////////////////

Int::Int (const String& o) {
	data = o.toInt();
}

int Int::operator == (const Comparable& o) const {
	if (o.is_a ("Int"))
		return data==((const Int&)o).data;

	return -1; // Undefined value. Should we throw an exception?
}



///////////////////////////////////////////////////////////////////////////////
// Apufunktioita


CPPModule::CPPModule (const char* n) {
	name = strdup (n);
	#ifdef MODULEDEBUG
	printf ("Module %s loading...\n", name);
	#endif
}


//

int rnd (int range) {
	return (int) (double(range)*rand()/(RAND_MAX+1.0));
}

double frnd () {
	return rand()/(RAND_MAX+1.0);
}

double gaussrnd (double stdv) {
	if (stdv>0.0) {
		double r = rand()/(RAND_MAX+1.0)-0.5;
		return stdv * log ((r+0.5)/(0.5-r));
	}
	return 0.0;
}

// From mmagisupp.h

Exception::Exception (const String& what_arg) : mWhat (new String (what_arg)) {
}

Exception::Exception (const Exception& orig) : mWhat (new String (*orig.mWhat)) {
}

Exception::~Exception () {
	delete mWhat;
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//              _                             o                             //
//             / \   ____  ____  ___       |           _    ____            //
//            /   \ (     (     /   ) |/\ -+- |  __  |/ \  (                //
//            |---|  \__   \__  |---  |    |  | /  \ |   |  \__             //
//            |   | ____) ____)  \__  |     \ | \__/ |   | ____)            //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

int assertmode=ASSERT_THROW;

/*******************************************************************************
* Handles assertion failure.
*******************************************************************************/
void assertwith (const char* filename,
				 int         lineno,
				 const char* funcname,
				 const char* condition,
				 const char* message)
{
	cout << strlen("Condition '%s' violated at %s[%d]: %s()") << "\n";
	cout << strlen(condition) << "\n";
	cout << strlen(filename) << "\n";
	cout << lineno << "\n";
	cout << strlen(funcname) << "\n";
	String errormsg = strformat("Condition '%s' violated at %s[%d]: %s()",
								condition, filename, lineno, funcname);

	if (message)
		errormsg += strformat ("\n%%ERR: %s", message);

	switch (assertmode) {
	  case ASSERT_CRASH:
		  fprintf (stderr, "\n%%ERR: %s\n", (CONSTR) errormsg);
		  fprintf (stderr, "%%ERR: Intentional assertion crash. "
				   "I hope you are under debugger now...\n");
		  *((int*)NULL) = 1; // So you're under debugger. Ok, now just backtrace
		  break;
	  case ASSERT_EXIT:
		  fprintf (stderr, "\n%%ERR: %s\n", (CONSTR) errormsg);
		  exit (1);
		  break;
	  case ASSERT_DONTCARE:
		  fprintf (stderr, "\n%%ERR: %s\n", (CONSTR) errormsg);
		  break;
	  case ASSERT_THROW:
	  default:
		  throw assertion_failed (format ("%s", (CONSTR)errormsg));
	};
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef DISABLE_ALL_MEMORY_DEBUGGING
#ifdef new
#undef new
#endif
#warning "Memory debugging enabled"

int dbgnewmalloced=0;
mallocinfostr* lastmalloced=NULL;
int newtrace_disabled = 1;
char* newcomment=NULL;

void newComment (const char* comment) {
	if (newcomment)
		free (newcomment);
	newcomment = strdup(comment);
}


void* operator new[] (size_t size, const char* filename, int lineno, const char* fname)
{
	// fprintf (stderr, "filename=%s, lineno=%d, fname=%s\n", filename, lineno, fname);

	if (newtrace_disabled) {
		void* res = malloc(size);
		if (!res)
			*((char*)0x0)=0;
		return res;
	}

	// Reserve the memory block PLUS space for the info structure
	mallocinfostr* p = (mallocinfostr*) malloc (size+sizeof(mallocinfostr));
	if (!p)
		*((char*)0x0)=0;

	strcpy (p->startsym, "MiS_S");	// Set a recognition label
	p->size			= size;
	p->filename		= filename;
	p->lineno		= lineno;
	p->funcname		= fname;
	p->newcomment	= newcomment? newcomment : NULL;
	newcomment		= NULL;
	p->renewed		= false;
	p->isObject		= nextNewIsObject;
	nextNewIsObject	= false;
	p->prev			= lastmalloced;
	p->next			= NULL;
	if (p->prev)
		p->prev->next = p;
	lastmalloced	= p;
	
	// Fill with 0x00
	memset (((char*)p)+sizeof (mallocinfostr), '\xcc', p->size);

	// Add to global counter
	dbgnewmalloced += size + sizeof(mallocinfostr);
	return ((char*)p)+sizeof (mallocinfostr);
}

void* operator new (size_t size, const char* filename, int lineno, const char* fname) {
	return operator new[] (size, filename, lineno, fname);
}

void* _renew (void* p, size_t newsize, const char* filename, int lineno, const char* fname) {
	if (memcmp (((char*)p)-sizeof(mallocinfostr), "MiS_S", 5) || newtrace_disabled) {
		void* res = realloc ((char*) p, newsize);
		// Again, should not be null
		if (!res)
			*((char*)0x0)=0;
		return res;
	} else {
		// Realloc the memory block PLUS space for the info structure
		mallocinfostr* np = (mallocinfostr*) realloc (((char*)p)-sizeof(mallocinfostr),
													  newsize+sizeof(mallocinfostr));
		// Again, should not be null
		if (!np)
			*((char*)0x0)=0;
		
		dbgnewmalloced -= np->size;	// Substract old size from total size register
		np->size = newsize;			// Refresh
		np->filename = filename;	// Refresh
		np->lineno = lineno;		// Refresh
		np->funcname = fname;		// Refresh
		if (newcomment)	{			// Refresh
			free (np->newcomment);
			np->newcomment = newcomment;
			newcomment = NULL;
		}
		np->renewed = true;			// Now we have been renewed
		if (np->prev)
			np->prev->next = np;
		if (np->next)
			np->next->prev = np;
		if ((void*) (((char*)p)-sizeof(mallocinfostr)) == (void*) lastmalloced)
			lastmalloced = np;
		dbgnewmalloced += newsize;	// Add new size to total size register
		return ((char*)np)+sizeof (mallocinfostr);
	}
}

void operator delete (void* p) {
	if (p) {
		if (memcmp (((char*)p)-sizeof(mallocinfostr), "MiS_S", 5)) {
			free (p); // Allocated with ordinary malloc or new
		} else {
			mallocinfostr* mp=(mallocinfostr*)(((char*)p)-sizeof(mallocinfostr));
			//TRACE3 ("%d, %s, %d", mp->size, mp->filename, mp->lineno);
			if (mp->next)
				mp->next->prev = mp->prev; 
			if (mp->prev)
				mp->prev->next = mp->next;
			if (mp == lastmalloced)
				if (mp->next)
					while (lastmalloced->next)
						lastmalloced=lastmalloced->next;
				else
					lastmalloced = mp->prev;
			if (mp->newcomment)
				free (mp->newcomment);
			dbgnewmalloced -= mp->size + sizeof (mallocinfostr);

			// Fill the block with 0xdd
			memset (mp, '\xdd', mp->size + sizeof (mallocinfostr));

			free (mp);
		}
	}
}

// int listAllocationsLimit=100;

void list_allocations (int number, FILE* out) {
	fprintf (out, "Dumping all reserved memory blocks:\n");
	int i=0;
	for (mallocinfostr* p=lastmalloced; p && (number==0 || i<number); p=p->prev, i++) {
		fprintf (out, "%9d bytes (%8x) %s at %s#%d %s",
				 p->size, (char*) (((char*)p) + sizeof (mallocinfostr)),
				 (p->renewed)? "renewed  ":"allocated",
				 p->filename, p->lineno, p->funcname);
		if (p->isObject) {
			Object* op = (Object*)(void*)(((char*)p)+sizeof(mallocinfostr));
			fprintf (out, " (%s)", (CONSTR) op->getclassname());
		}
		if (p->newcomment)
			fprintf (out, " : \042%s\042", (CONSTR) p->newcomment);
		fprintf (out, "\n");
	}
	if (number>0 && i>=number)
		fprintf (out, "** Only %d first items listed\n", number);
	fprintf (out, "Total amount of memory allocated: %d "
			 "(including the DEBUG_NEW overhead)\n", dbgnewmalloced);
}

#endif

END_NAMESPACE;
