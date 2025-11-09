/***************************************************************************
 *   This file is part of the MagiC++ library.                             *
 *                                                                         *
 *   Copyright (C) 1998-2002 Marko Gr�nroos <magi@iki.fi>                  *
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

#ifndef __MAGIC_MEXCEPTION_H__
#define __MAGIC_MEXCEPTION_H__

#include "magic/mobject.h"

namespace MagiC {
	class String;
}

BEGIN_NAMESPACE (MagiC);

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                -----                          o                           //
//                |          ___   ___   --   |           _                  //
//                |---  \ / |   \ /   ) |  ) -+- |  __  |/ \                 //
//                |      X  |     |---  |--   |  | /  \ |   |                //
//                |____ / \  \__/  \__  |      \ | \__/ |   |                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Exception handling macros

/** Highest exception class. Replacement for standard C++ 'exception'
 *  class. I think there was some reason to replace it.
 *
 *  You can subclass this baseclass with the EXCEPTIONCLASS()
 *  macro. For example:
 *
 *  EXCEPTIONCLASS (assertion_failed);
 *
 *  See mmagisupp.h for some common exception classes. Many debugging
 *  macros that throw exceptions are also defined there.
 *
 *  Exception fails can be tracked with the FAILTRACE macro.
 **/
class Exception : public Object {
	String*	mWhat;
  public:

	/** Constructor the Exception with the given explanation.
	 *
	 *  Example:
	 *
	 *  throw Exception ("Some text");
	 **/
					Exception	(const String& what);
					Exception	(const Exception& orig);
					~Exception	();

	/** Returns the explanation text given at throw.
	 **/
	const String&	what		() const {return *mWhat;}
};

#define EXCEPTIONCLASS(eclass) \
class eclass : public Exception {\
public:\
	eclass (const String& what_arg) : Exception (what_arg) { }\
	virtual ~eclass () {;}\
};

//  eclass (const __string& what_arg): logic_error (what_arg) {;}

/*******************************************************************************
* Common exceptions
*******************************************************************************/
EXCEPTIONCLASS (generic_exception);
EXCEPTIONCLASS (invalid_format);
EXCEPTIONCLASS (assertion_failed);
EXCEPTIONCLASS (forwarded_failure);
EXCEPTIONCLASS (file_not_found);		/**< A file was not found. */
EXCEPTIONCLASS (system_failure);		/**< A failure in an I/O or other system call. */
EXCEPTIONCLASS (open_failure);			/**< Opening a file or device failed for some reason. */
EXCEPTIONCLASS (invalid_flags);			/**< Flag parameters given in a call were invalid. */
EXCEPTIONCLASS (file_not_open);			/**< A file operation was attempted on a closed file. */
EXCEPTIONCLASS (device_not_open);		/**< An operation was attempted on a closed device. */
EXCEPTIONCLASS (stream_failure);		/**< Operation on a stream failed. */
EXCEPTIONCLASS (end_of_stream);			/**< End of stream was reached. */
EXCEPTIONCLASS (check_failed);			/**< Intergrity check operation for an object failed. */
EXCEPTIONCLASS (out_of_bounds);			/**< Requested collection item was out of bounds. */
EXCEPTIONCLASS (out_of_range);
EXCEPTIONCLASS (runtime_error);
EXCEPTIONCLASS (io_error);              /**< Generic error in I/O operation. */

/*******************************************************************************
* Special exceptions for unimplemented member functions
*******************************************************************************/

// For abstract classes that can't use =0
EXCEPTIONCLASS (must_overload);
#define MUST_OVERLOAD {assertwith (__FILE__, __LINE__, __FUNCTION__, "MUST_OVERLOAD", format("%%ERR: Method of baseclass must be overloaded by descendant %s", typeid(*this).name()));}

EXCEPTIONCLASS (forbidden_call);
#define FORBIDDEN {throw forbidden_call (format("%%ERR: Forbidden execution at %s[%d]: %s()", __FILE__, __LINE__, __FUNCTION__));}

EXCEPTIONCLASS (not_implemented);
#define NOT_IMPLEMENTED {throw not_implemented (format("%%ERR: Function not implemented at %s[%d]: %s()", __FILE__, __LINE__, __FUNCTION__));}

/*******************************************************************************
* Failtrace and trywith
*******************************************************************************/

#define failtrace_begin try {
#define failtrace_end \
	} catch (Exception e) { \
		throw forwarded_failure (format ("%s\n%%ERR: Exception forwaded at %s[%d]:%s()", \
										 (CONSTR) e.what(), __FILE__, __LINE__, __FUNCTION__)); \
	}

#define FUNCTION_BEGIN failtrace_begin
#define FUNCTION_END   failtrace_end

/** If the expr throws an exception, this macro rethrows it, but
 *  records the filename and line number in the Exception reason.
 **/
#define failtrace(expr) failtrace_begin; expr; failtrace_end;

#define FAILTRACE failtrace

/** Much like failtrace(expr). If the expr throws an exception, this
 *  macro rethrows it, but records the filename and line number in the
 *  Exception reason, as well as additional explanation.
 **/
#define trywith(expr,err) \
	try { \
		expr; \
	} catch (Exception e) { \
		throw forwarded_failure (format ("%s\n%%ERR: Exception forwaded at %s[%d]:%s()\n%%ERR: %s", \
										 (CONSTR) e.what(), __FILE__, __LINE__, __FUNCTION__, (CONSTR) err)); \
	}

#define TRYWITH trywith

/*******************************************************************************
 *  The @ref Application and ASSERT framework supports many ways of
 *  handling assertion fails. It is, for example, very useful to crash
 *  the program (ASSERT_CRASH) if you are running the program under
 *  debugger.
 *
 *  Just include in your Main line:
 *
 *  assertmode = ASSERT_CRASH;
 ******************************************************************************/
enum assertmodes {ASSERT_THROW=0, ASSERT_CRASH=1, ASSERT_EXIT=2, ASSERT_DONTCARE=3};

extern FILE* dbgout;

extern int assertmode;

/** Internal for ASSERT and ASSERTWITH. **/
// extern void assertwith (const char* filename, int lineno, const char* funcname,
//						const char* condition=NULL, const char* message=NULL);

END_NAMESPACE;

#endif
