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
#include "magic/mturtle.h"


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//           -----               |        ----                               //
//             |              |  |  ___  (      |   ___   |   ___            //
//             |   |   | |/\ -+- | /   )  ---  -+-  ___| -+- /   )           //
//             |   |   | |    |  | |---      )  |  (   |  |  |---            //
//             |    \__! |     \ |  \__  ___/    \  \__|   \  \__            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

void TurtleState::copy (const TurtleState& o) {
	mCoord = o.mCoord;
	mRot = o.mRot;
}



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                        -----               |                              //
//                          |              |  |  ___                         //
//                          |   |   | |/\ -+- | /   )                        //
//                          |   |   | |    |  | |---                         //
//                          |    \__! |     \ |  \__                         //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

void Turtle::forward () {
	double x1 = mCoord.x + mStepSize*cos(mRot);
	double y1 = mCoord.y + mStepSize*sin(mRot);
	mDevice.forwardLine (mCoord, Coord2D(x1,y1));
	jumpTo (x1, y1);
}

void Turtle::drawLSystem (const String& str) {
	Array<TurtleState> stack (1000);
	int stackp=0;
	bool goingForward=false; // Needed to detect tip points

	mDevice.start ();

	// Scan the string char by char
	for (uint i=0; i<str.length(); i++) {
		// TRACE1 ("%c", str[i]);
		// serr << *this;
		// serr.print("\n");
		switch (str[i]) {
		  case 'F': {
			  forward ();
		  } break;
		  case '[': { // PUSH
			  ASSERTWITH (stackp < stack.size()-1,
						  format ("Stack (size %d) overflow", stack.size()));
			  stack[stackp++].copy (*this);
			  goingForward = true;
		  } break;
		  case ']': { // POP
			  // Draw tip point
			  if (goingForward) {
				  mDevice.tip (mCoord);
				  goingForward = false;
			  }
			  ASSERTWITH (stackp>0, "Stack underflow, too much ]'s");
			  this->copy (stack[--stackp]);
		  } break;
		  case '+': {
			  turnBy (mDeltaAngle);
		  } break;
		  case '-': {
			  turnBy (-mDeltaAngle);
		  } break;
		  default:
			  ;
		}
	}

	mDevice.end ();
}

OStream& Turtle::operator>> (OStream& out) const
{
	out.printf ("{(%d,%d) rot=%f step=%d drot=%f}",
				int(mCoord.x), int(mCoord.y), mRot,
				mStepSize, mDeltaAngle);
	return out;
}
