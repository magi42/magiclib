#ifndef __GDEV_EPS_H__
#define __GDEV_EPS_H__

#include "magic/mgobject.h"
#include "magic/mcoord.h"

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

/** Encapsulated PostScript graphics driver.
 **/
class EPSDevice {
  public:
	
					EPSDevice		() {make ();}
					EPSDevice		(const Coord2D& dimensions);
	virtual			~EPSDevice		() {}

	void			make			();
	
	// Implementations

	virtual EPSDevice&	setPixel		(const Coord2D&) {NOT_IMPLEMENTED; return *this;}
	virtual EPSDevice&	line			(const Coord2D& start, const Coord2D& end);
	virtual EPSDevice&	circle			(const Coord2D& pos, float r, bool fill=false);
	virtual EPSDevice&	rect			(const Rect& rect);
	virtual EPSDevice&	setClipping		(const Rect& rect);
	virtual EPSDevice&	endClipping		();
	virtual EPSDevice&	text			(const String& text, float x, float y) {NOT_IMPLEMENTED; return *this;}
	virtual EPSDevice&	setGray			(float g) {mStr+=format("%f setgray\n", g); return *this;}

	virtual EPSDevice&	saveState		() {mStr+="gsave\n"; return *this;}
	virtual EPSDevice&	restoreState	() {mStr+="grestore\n"; return *this;}
	virtual EPSDevice&	lineStyle		(const String& style, float scale=1.0);
	virtual EPSDevice&	lineWidth		(float w) {mStr+=format ("%f setlinewidth\n", w); return *this;}
	virtual EPSDevice&	scaling			(float x, float y);
	virtual EPSDevice&	origin			(const Coord2D& pos);

	virtual EPSDevice&	framedStyle		(float xUserSize, float yUserSize);
	virtual EPSDevice&	flipCoordinates	();
	
	// Informative
	
	float			width			() const {return mDimensions.x;}
	float			height			() const {return mDimensions.y;}
	const Coord2D&	scaling			() const {return mScaling;}
	
	// Own methods

	EPSDevice&		directPrint		(CONSTR str);
	EPSDevice&		comment			(CONSTR str) {mStr+=str; return *this;}
	
	// Adds footer to buffer
	void			printFooter		();

	String			getBuffer		() const {return mStr;}

  private:
	Coord2D			mDimensions;
	Rect			mBoundingBox;
	Coord2D			mScaling, mOffset;
	String			mStr;
	bool			mArrowLines;
	float			mArrowHeadSize;
	
	void			printHeader		();
	void			checkRange		(const Coord2D& pos);
};

END_NAMESPACE;

#endif
