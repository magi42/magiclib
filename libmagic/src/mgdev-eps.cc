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

#include "magic/mgdev-eps.h"

BEGIN_NAMESPACE (MagiC);


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//             ----- ----   ---- ___               o                         //
//             |     |   ) (     |  \   ___           ___   ___              //
//             |---  |---   ---  |   | /   ) |   | | |   \ /   )             //
//             |     |         ) |   | |---   \ /  | |     |---              //
//             |____ |     ___/  |__/   \__    V   |  \__/  \__              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

EPSDevice::EPSDevice (const Coord2D& dimensions) {
	make ();
	mDimensions = dimensions;
	mBoundingBox.set (0,0, dimensions.x, dimensions.y);
}

void EPSDevice::make () {
	mDimensions.moveTo (0,0);
	mBoundingBox.moveTo (Coord2D(0,0));
	mStr="";
	mScaling.moveTo (1,1);
	mOffset.moveTo (0,0);
	mArrowLines = false;
	mArrowHeadSize = 10.0;
}

void EPSDevice::printFooter () {
	printHeader ();
}

void EPSDevice::printHeader () {
	String header = "%!PS-Adobe-2.0 EPSF\n";
	header += format ("%%%%BoundingBox: 0 0 %f %f\n",
					  mBoundingBox.upperRight().x, mBoundingBox.upperRight().y);
	header += "%% Author info: C++ EPSDevice driver by Marko Gronroos (magi@utu.fi)\n"
		"%%Pages: 0\n"
		"%%EndComments\n\n"
		"% Draws a line with an arrow head\n"
		"/arrowline { % x0 y1 x1 y1 headsize\n"
		"	/headscale exch def			% Store variables\n"
		"	/y1 exch def /x1 exch def\n"
		"	/y0 exch def /x0 exch def\n"
		"	newpath x0 y0 moveto x1 y1 lineto	% Draw basic line\n"
		"	y1 y0 sub x1 x0 sub atan		% Calculate angle\n"
		"	165 add					% Change the angle\n"
		"	dup\n"
		"	cos headscale mul x1 add		% Calc next point\n"
		"	exch sin headscale mul y1 add\n"
		"	lineto					% Draw to it\n"
		"	y1 y0 sub x1 x0 sub atan		% Calculate angle\n"
		"	165 sub					% Change the angle\n"
		"	dup\n"
		"	cos headscale mul x1 add\n"
		"	exch sin headscale mul y1 add\n"
		"	lineto x1 y1 lineto fill\n"
		"} def\n\n"
		"0 setlinecap 0 setlinejoin\n"
		"0.500 setlinewidth\n";

	// Put the header into the drawing string
	header += mStr;
	mStr = header;
}

EPSDevice& EPSDevice::line (const Coord2D& start, const Coord2D& end) {
	checkRange (start);
	checkRange (end);
	if (mArrowLines)
			mStr += format ("%f %f %f %f %f arrowline\n",
							start.x, start.y, end.x, end.y, mArrowHeadSize);
		else
			mStr += format ("newpath %f %f moveto %f %f lineto stroke\n",
							start.x, start.y, end.x, end.y);
	return *this;
}

EPSDevice& EPSDevice::circle (const Coord2D& center, float r, bool fill) {
	checkRange (center+Coord2D(r,r));
	mStr += format ("newpath %f %f %f 0 360 arc gsave stroke grestore\n",
					center.x, center.y, r );
	if (fill)
		mStr += "fill\n";
	return *this;
}

EPSDevice& EPSDevice::rect (const Rect& rect) {
	checkRange (rect.lowerLeft());
	checkRange (rect.upperRight());
	mStr += format ("newpath %f %f moveto %f %f lineto "
					"%f %f lineto %f %f lineto %f %f lineto closepath stroke\n",
					rect.lowerLeft().x, rect.lowerLeft().y,
					rect.lowerLeft().x, rect.upperRight().y,
					rect.upperRight().x, rect.upperRight().y,
					rect.upperRight().x, rect.lowerLeft().y,
					rect.lowerLeft().x, rect.lowerLeft().y);
	return *this;
}

EPSDevice& EPSDevice::setClipping (const Rect& rect) {
	checkRange (rect.lowerLeft());
	checkRange (rect.upperRight());
	mStr += format ("%f %f %f %f rectclip  %% Clipping ON\n",
					rect.lowerLeft().x, rect.lowerLeft().y, rect.width(), rect.height());
	return *this;
}

EPSDevice& EPSDevice::endClipping () {
	mStr += "eoclip% End-Of-Clipping\n";
	return *this;
}

EPSDevice& EPSDevice::lineStyle (const String& style, float scale) {
	if (style == "solid")
		mStr += "[ ] 0 setdash % solid\n";
	else if (style == "dashed")
		mStr += format ("[%f] 0 setdash %% dashed\n", 3*scale);
	else if (style == "dotted")
		mStr += format ("[%f] %f setdash %% dotted\n", 5*scale, scale);
	else if (style == "->")
		mArrowLines = true, mArrowHeadSize=scale;
	else if (style == "-")
		mArrowLines = false;
	else
		ASSERTWITH (false, format ("Unrecognized line style '%s'", (CONSTR) style));
	return *this;
}

EPSDevice& EPSDevice::scaling (float x, float y) {
	mScaling.moveTo (x, y);
	mStr+=format("%f %f scale\n",x,y);
	return *this;
}

EPSDevice& EPSDevice::origin (const Coord2D& pos) {
	mOffset.copy (pos);
	mStr+=format("%f %f translate\n",pos.x,pos.y);
	return *this;
}

EPSDevice& EPSDevice::framedStyle (float xUserSize, float yUserSize) {
	flipCoordinates ();
	rect (Rect (-1,-1, mDimensions.x, mDimensions.y));			// Picture border
	setClipping (Rect (0,0, mDimensions.x-1, mDimensions.y-1));	// Clipping inside the bordera
	origin (Coord2D(1,1));
	scaling (mDimensions.x/xUserSize, mDimensions.y/yUserSize);
	return *this;
}

EPSDevice& EPSDevice::flipCoordinates () {
	origin (Coord2D(0,mDimensions.y));		// Move origin to previous upper-left corner
	scaling (1, -1);						// Flip coordinates
	return *this;
}

void EPSDevice::checkRange (const Coord2D& pos) {
	if (pos.x>mBoundingBox.upperRight().x)	mBoundingBox.upperRight().x=pos.x;
	if (pos.y>mBoundingBox.upperRight().y)	mBoundingBox.upperRight().y=pos.y;
	mStr.ensure_spontane (mStr.length()+200);
}

EPSDevice& EPSDevice::directPrint (CONSTR str) {
	mStr += str;
	return *this;
}

END_NAMESPACE;
