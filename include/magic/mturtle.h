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

#ifndef __MAGIC_MTURTLE_H__
#define __MAGIC_MTURTLE_H__

#include <magic/mobject.h>
#include <magic/mcoord.h>


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//        -----               |       ___               o                    //
//          |              |  |  ___  |  \   ___           ___   ___         //
//          |   |   | |/\ -+- | /   ) |   | /   ) |   | | |   \ /   )        //
//          |   |   | |    |  | |---  |   | |---   \ /  | |     |---         //
//          |    \__! |     \ |  \__  |__/   \__    V   |  \__/  \__         //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/** Abstract interface for turtle drawing devices. Implementors are
 *  usually device-specific.
 **/
class TurtleDevice : public Object {
  public:
					TurtleDevice		() {;}

	/** Starts drawing. */
	virtual void	start				() {;}

	/** Draws a line forward.
	 *
	 *  @param start Start coordinates of the line.
	 *  @param end End coordinates of the line.
	 **/
	virtual void	forwardLine			(const Coord2D& start, const Coord2D& end) {} //=0;

	/** To draw a "tip". Especially useful for drawing LSystems
	 *  produced by @ref LGrammar.
	 **/
	virtual void	tip					(const Coord2D& point) {;}

	/** End drawing. */
	virtual void	end					() {;}
};



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//           -----               |        ----                               //
//             |              |  |  ___  (      |   ___   |   ___            //
//             |   |   | |/\ -+- | /   )  ---  -+-  ___| -+- /   )           //
//             |   |   | |    |  | |---      )  |  (   |  |  |---            //
//             |    \__! |     \ |  \__  ___/    \  \__|   \  \__            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/** Saved location and orientation state of a turtle. These are placed
 *  in stack by @ref Turtle.
 **/
class TurtleState : public Object {
  public:
				TurtleState		() {mRot=0.0;}

	/** Moves to given coordinates. */
	void		jumpTo			(double x, double y) {mCoord.moveTo (x,y);}
	/** Moves to given coordinates. */
	void		jumpTo			(const Coord2D& pos) {mCoord.copy (pos);}

	/** Turns the turtle by given amount (in radians). */
	void		turnBy			(double rot) {mRot+=rot;}

	/** Copy operator. */
	void		copy			(const TurtleState& o);
	/** Copy operator. */
	void		operator=		(const TurtleState& o) {copy(o);}

  protected:
	/** Coordinates. */
	Coord2D	mCoord;
	
	/** Current rotation angle. */
	double	mRot;
};


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                        -----               |                              //
//                          |              |  |  ___                         //
//                          |   |   | |/\ -+- | /   )                        //
//                          |   |   | |    |  | |---                         //
//                          |    \__! |     \ |  \__                         //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/** Turtle for drawing pictures. */
class Turtle : public TurtleState {
  public:
	/** Standard constructor.
	 *
	 *  @param dev Graphics device.
	 *  @param step Step scale size (for example 10 units).
	 *  @param drot Rotation delta, in degrees (for example 45 degrees).
	 **/
				Turtle			(TurtleDevice& dev, double step, double drot) : mDevice (dev) {
					mStepSize = step;
					mDeltaAngle = drot*M_PI/180.0;
					jumpTo (0,0);
					mRot = 0.0;
				}

	/** Draws the given L-System description string produced by @ref LGrammar.
	 *
	 *  Standard commands for L-Systems are accepted:
	 *
	 *  F = forward one 'step' length (see the constructor), pen down.
	 *
	 *  f = forward one 'step' length (see the constructor), pen up.
	 *
	 *  + = turn right by the 'drot' amount given in constructor.
	 *
	 *  - = turn left by the 'drot' amount given in constructor.
	 *
	 *  [ = push the current location into stack.
	 *
	 *  ] = pop the topmost location from the stack.
	 **/
	void		drawLSystem		(const String& lstring);

	/** Moves the turle forward one 'step' length (see the constructor).
	 **/
	void		forward			();

	/** Implementation for @ref Object. Dumps the object description
	 *  to the given stream.
	 **/
	virtual OStream&	operator>>		(OStream& out) const;

  protected:
	TurtleDevice&	mDevice;		// Drawing device
	double			mStepSize;		// Size of a step
	double			mDeltaAngle;	// Turning angle, in radians
};

#endif
