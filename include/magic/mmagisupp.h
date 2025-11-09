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

#ifndef __MAGIC_MMAGISUPP_H__
#define __MAGIC_MMAGISUPP_H__

#include "magic/mdebug.h"

#ifndef BEGIN_NAMESPACE
#define BEGIN_NAMESPACE(ns) namespace ns {
#ifdef QT_VERSION
#define END_NAMESPACE }
#else
#define END_NAMESPACE } using namespace MagiC;
#endif
#endif

///////////////////////////////////////////////////////////////////////////////
// Debugging macros

/** Assertion mode. See @ref assertmodes in mmagisupp.h
 **/

#include "magic/mdebug.h"

#ifdef MSDOS
#define IOSBINARY binary
#else // UNIX
#define IOSBINARY bin
#endif

BEGIN_NAMESPACE (MagiC);

/*******************************************************************************
 * Screen manipulation (minicurses)
 ******************************************************************************/
#define clrscr() {cout << "\033[H\033[J";}
#define randomize	{srand (time (NULL));}
//#define rnd(range) {return 

END_NAMESPACE;
#endif


