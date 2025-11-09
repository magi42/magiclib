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

#ifndef __MAGIC_MREGEXP_H__
#define __MAGIC_MREGEXP_H__

#include <sys/types.h>
#include <regex.h>

// Externals:

namespace MagiC {
	// Externals
	class String;

	// Internals
	class RegExp;
}
BEGIN_NAMESPACE (MagiC);

template <class TYPE> class Array;


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                     ----              -----                              //
//                     |   )  ___        |          --                      //
//                     |---  /   )  ___  |---  \ / |  )                     //
//                     | \   |---  (   \ |      X  |--                      //
//                     |  \   \__   ---/ |____ / \ |                        //
//                                  __/                                     //
//////////////////////////////////////////////////////////////////////////////

class RegExp { // : public Object
	
	regex_t*	regt;
  public:
	// Mahdolliset käännöksissä tai vertailuissa havaitut virheet
	int			errcode;

				RegExp		() {regt=NULL; errcode=0;}
				RegExp		(const char* expr) {regt=NULL; errcode=0; make (expr);}
				~RegExp		();

	// Esikääntää ekspression. Palauttaa !=0 virheen sattuessa
	int			make		(const char* expr);

	// Palauttaa 1 jos sovitus onnistui
	int			match		(const char* string);

	// Kuten yllä, mutta tallentaa aliekspressioiden tulokset results-vektoriin
	int			match		(const String& string, Array<String>& results);

	// Palauttaa virheen kuvauksen merkkijonona
	String		geterror	() const;
};

END_NAMESPACE;

#endif
