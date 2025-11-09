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

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <cstddef>

// Remove new-redefinition
#ifdef new
#undef new
#define new_UNDEFD
#endif

// Restore new-redefinition
#ifdef new_UNDEFD
#define new DEBUG_NEW
#undef new_UNDEFD
#endif

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// ___                               ----               o                   //
// |  \   ___  |                    (      ___   |   |      _          ____ //
// |   | /   ) |---  |   |  ___      ---  /   ) -+- -+- | |/ \   ___  (     //
// |   | |---  |   ) |   | (   \        ) |---   |   |  | |   | (   \  \__  //
// |__/   \__  |__/   \__!  ---/    ___/   \__    \   \ | |   |  ---/ ____) //
//                          __/                                  __/        //
//////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * Enables lots of debugging
 ******************************************************************************/
#define DEBUG_MODE

// TODO: Handle this somehow...
#ifndef DEBUG_MODE
#define NODEBUG
#endif

/*******************************************************************************
 * Enables tracking of memory allocation, deletion, and memory leaks.
 * Note: Must be also defined in mmagisupp.h
 ******************************************************************************/
//#define new DEBUG_NEW

/*******************************************************************************
 * Enables tracking the inheritors of the Object superclass. May cause problems.
 ******************************************************************************/
// #define DEBUG_OBJECT_NEW

/*******************************************************************************
 * This disables ALL memory debugging from all modules
 ******************************************************************************/
#ifndef MEMORY_DEBUG
#define DISABLE_ALL_MEMORY_DEBUGGING
#else
#define new DEBUG_NEW
#endif

/*******************************************************************************
 * Define system
 ******************************************************************************/
#define LINUX
#define UNIX


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//     ___                                                                   //
//     |  \   ___  |                           ___   ___            ____     //
//     |   | /   ) |---  |   |  ___     |/|/|  ___| |   \ |/\  __  (         //
//     |   | |---  |   ) |   | (   \    | | | (   | |     |   /  \  \__      //
//     |__/   \__  |__/   \__!  ---/    | | |  \__|  \__/ |   \__/ ____)     //
//                              __/                                          //
///////////////////////////////////////////////////////////////////////////////

#ifndef NODEBUG

#define TRACELINE {fprintf (dbgout, "File:%s, Line#%d %s\n", __FILE__, __LINE__, __FUNCTION__); fflush (dbgout);}
#define TRACE(expr) {fprintf (dbgout, "File:%s, Line#%d %s: ", __FILE__, __LINE__, __FUNCTION__); fprintf (dbgout, expr); fprintf (dbgout, "\n"); fflush (dbgout);}
#define TRACE1(expr,par1) {fprintf (dbgout, "File:%s, Line#%d %s: ", __FILE__, __LINE__, __FUNCTION__); fprintf (dbgout, expr, par1); fprintf (dbgout, "\n"); fflush (dbgout);}
#define TRACE2(expr,par1,par2) {fprintf (dbgout, "File:%s, Line#%d %s: ", __FILE__, __LINE__, __FUNCTION__); fprintf (dbgout, expr, par1, par2); fprintf (dbgout, "\n"); fflush (dbgout);}
#define TRACE3(expr,par1,par2,par3) {fprintf (dbgout, "File:%s, Line#%d %s: ", __FILE__, __LINE__, __FUNCTION__); fprintf (dbgout, expr, par1, par2, par3); fprintf (dbgout, "\n"); fflush (dbgout);}
#define TRACE4(expr,par1,par2,par3,par4) {fprintf (dbgout, "File:%s, Line#%d %s: ", __FILE__, __LINE__, __FUNCTION__); fprintf (dbgout, expr, par1, par2, par3, par4); fprintf (dbgout, "\n"); fflush (dbgout);}
#define TRACE5(expr,par1,par2,par3,par4,par5) {fprintf (dbgout, "File:%s, Line#%d %s: ", __FILE__, __LINE__, __FUNCTION__); fprintf (dbgout, expr, par1, par2, par3, par4, par5); fprintf (dbgout, "\n"); fflush (dbgout);}
#define TRACE6(expr,par1,par2,par3,par4,par5,par6) {fprintf (dbgout, "File:%s, Line#%d %s: ", __FILE__, __LINE__, __FUNCTION__); fprintf (dbgout, expr, par1, par2, par3, par4, par5, par6); fprintf (dbgout, "\n"); fflush (dbgout);}

/** Internal for ASSERT and ASSERTWITH. **/
namespace MagiC {
extern void assertwith (const char* filename, int lineno, const char* funcname,
						const char* condition=NULL, const char* message=NULL);
}

// ASSERT is not available under Qt, which also defines it. Clever.
#ifndef QT_VERSION
#define ASSERT(expr) {if (!(expr)) MagiC::assertwith (__FILE__, __LINE__, __FUNCTION__, #expr, "Assertion failed");}
#endif

#define ASSERTWITH(expr,message) {if (!(expr)) MagiC::assertwith (__FILE__, __LINE__, __FUNCTION__, #expr, message);}
#define PRE(expr) ASSERTWITH(expr, "Violation of precondition");
#define POST(expr) ASSERTWITH(expr, "Violation of postcondition");

#define IFDEBUG(expr) {expr;}

#else

#define TRACELINE
#define ASSERT(expr)
#define ASSERTWITH(expr,message)
#define PRE(expr)
#define POST(expr)
#define TRACE(msg) {}
#define TRACE1(msg,p1) {}
#define TRACE2(msg,p1,p2) {}
#define TRACE3(msg,p1,p2,p3) {}
#define TRACE4(msg,p1,p2,p3,p4) {}
#define IFDEBUG(expr) {expr;}
#endif



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//     |   |                                   |                             //
//     |\ /|  ___                              |  ___  |                     //
//     | V | /   ) |/|/|  __  |/\ \   |     ---| /   ) |---  |   |  ___      //
//     | | | |---  | | | /  \ |    \  |    (   | |---  |   ) |   | (   \     //
//     |   |  \__  | | | \__/ |     \_/     ---|  \__  |__/   \__!  ---/     //
//                                 \_/                              __/      //
///////////////////////////////////////////////////////////////////////////////

// Define may memory debugging extensions, unless disabled in config.h
#ifndef DISABLE_ALL_MEMORY_DEBUGGING

//#define new DEBUG_NEW
#define DEBUG_NEW new(__FILE__,__LINE__,__FUNCTION__)
#ifdef new
#undef new
#define new_UNDEFD
#define renew(p,ns) _renew (p,ns,__FILE__,__LINE__,__FUNCTION__)
#else
#define renew realloc
#warning "Renew is realloc"
#endif

extern void* operator new[] (size_t size, const char* filen, int lineno, const char* funcn);
extern void* operator new (size_t size, const char* filen, int lineno, const char* funcn);
extern void operator delete (void* p);
extern void list_allocations (int number=100, FILE* out=stderr);
extern void* _renew (void* p, size_t newsize, const char* filen, int lineno, const char* funcn);
extern "C" void newComment (const char* comment);

#ifdef new_UNDEFD
#warning DEBUG_NEW enabled
#define new DEBUG_NEW
#undef new_UNDEFD
#endif

struct mallocinfostr {
	char			startsym[6];
	int 			size;
	const char*		filename;
	int				lineno;
	const char*		funcname;
	char*			newcomment;
	bool			renewed;
	bool			isObject;
	mallocinfostr*	next;
	mallocinfostr*	prev;

	// This is required to corrent double-allocation word frame shift,
	// which results in BUS ERROR when writing to a double reference
	char			padding[4];
};

extern mallocinfostr* lastmalloced;		// The last malloced memory block
extern int dbgnewmalloced;				// Total amount of memory malloced
extern int newtrace_disabled;

#else // DISABLE_ALL_MEMORY_DEBUGGING
// Some dummy function definitions

#include <stdlib.h>

inline void* _renew (void* p, size_t newsize, const char* filen, int lineno, const char* funcn) {
	return realloc (p, newsize);
}
#define renew(p,ns) _renew (p,ns,__FILE__,__LINE__,__FUNCTION__)

inline void newComment (const char* comment) {}

#endif // DISABLE_ALL_MEMORY_DEBUGGING

#endif
