#include "magic/mgobject.h"

BEGIN_NAMESPACE (MagiC);

decl_module (gobject);

void Rect::operator= (const Rect& other) {
	memcpy (this, &other, sizeof(Rect));
}

void Rect::moveTo (const Coord2D& pos) {
	Coord2D offset (pos.x-mCorner1.x, pos.y-mCorner1.y);
	moveBy (offset);
}

void Rect::moveBy (const Coord2D& offset) {
	mCorner1 += offset;
	mCorner2 += offset;
}

bool Rect::touches (const Rect& other) const {
	/*
	if (is_inside (other.x0, other.y0))	return 1;
	if (is_inside (other.x0, other.y1))	return 1;
	if (is_inside (other.x1, other.y1))	return 1;
	if (is_inside (other.x1, other.y0))	return 1;
	if (other.is_inside (x0, y0))	return 1;
	if (other.is_inside (x0, y1))	return 1;
	if (other.is_inside (x1, y1))	return 1;
	if (other.is_inside (x1, y0))	return 1;
	*/
	return false;
}

END_NAMESPACE;
