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

#include <stdio.h>
#include "magic/mlsystem.h"



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                |      ----                                                //
//                |     |          ___               ___                     //
//                |     | --- |/\  ___| |/|/| |/|/|  ___| |/\                //
//                |     |   \ |   (   | | | | | | | (   | |                  //
//                |____ |___/ |    \__| | | | | | |  \__| |                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

void LGrammar::addRule (const String& left, const String& right) {
	rules.add (new LRule (left, right));
}

void LGrammar::applyTo (String& str, int times) const {
	for (int t=0; t<times; t++) {
		String nextstr = "";
		// Scan through the string
		for (uint pos=0; pos < str.length(); pos++) {
			// Try to match each rule to this position
			bool matched = false;
			for (int rule=0; rule<rules.size(); rule++) {
				const String& left = rules[rule].left;
				if (str.mid (pos, left.length()) == left) {
					// Matched. Now replace.
					nextstr += rules[rule].right;
					matched = true;
					break;
				}
			}
			
			// If no match, just copy the character
			if (!matched)
				nextstr += str.mid (pos, 1);
		}
		str = nextstr;
	}
}
