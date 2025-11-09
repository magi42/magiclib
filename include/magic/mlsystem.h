/***************************************************************************
 *   This file is part of the MagiC++ library.                             *
 *                                                                         *
 *   Copyright (C) 1998-2004 Marko Grönroos <magi@iki.fi>                  *
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

#ifndef __MAGIC_MLSYSTEM_H__
#define __MAGIC_MLSYSTEM_H__

#include <magic/mobject.h>
#include <magic/mstring.h>
#include <magic/mpararr.h>

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                         |     ----        |                               //
//                         |     |   )       |  ___                          //
//                         |     |---  |   | | /   )                         //
//                         |     | \   |   | | |---                          //
//                         |____ |  \   \__! |  \__                          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/** L-System rule. INTERNAL of @ref LGrammar.
 **/
class LRule : public Object {
  public:
	/** Left-hand side of the production rule. */
	String left;
	
	/** Right-hand side of the production rule. */
	String right;
	
			LRule	() {;}

	/** Standard constructor.
	 *
	 **/
			LRule	(const String& l, const String& r) : left(l), right (r) {;}
};



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                |      ----                                                //
//                |     |          ___               ___                     //
//                |     | --- |/\  ___| |/|/| |/|/|  ___| |/\                //
//                |     |   \ |   (   | | | | | | | (   | |                  //
//                |____ |___/ |    \__| | | | | | |  \__| |                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/** L-System grammar.
 **/
class LGrammar : public Object {
	Array<LRule>	rules;
  public:
				LGrammar	() {;}

	/** Adds a production rule.
	 *
	 *  @param left Left-hand side of the production rule.
	 *  @param right Right-hand side of the production rule.
	 **/
	void		addRule		(const String& left, const String& right);

	/** Applies the grammar to the given string.
	 *
	 *  @param times Number of times the grammar is applied.
	 **/
	void		applyTo		(String& src, int times=1) const;
};

#endif
