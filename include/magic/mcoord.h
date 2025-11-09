#ifndef __COORD_H__
#define __COORD_H__

#include "magic/mmath.h"
#include "magic/mstream.h"

BEGIN_NAMESPACE (MagiC);

class Coord2D;
class Coord3D;

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                    ___                    |  __  ___                     //
//                   /   \                   | /  \ |  \                    //
//                   |      __   __  |/\  ---|   _/ |   |                   //
//                   |     /  \ /  \ |   (   |  /   |   |                   //
//                   \___/ \__/ \__/ |    ---| /___ |__/                    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

/** Two-dimensional coordinates in float space.
 **/
class Coord2D /*: public Object*/ {
  public:
	float	x, y;

					Coord2D		(float x0=0, float y0=0) : x(x0), y(y0) {;}
					Coord2D		(const Coord3D& o) {copy(o);}

	Coord2D&		operator=	(const Coord2D& o) {copy(o); return *this;}
	void			operator+=	(const Coord2D& o) {x+=o.x; y+=o.y;}
	Coord2D			operator+	(const Coord2D& o) const {return Coord2D(x+o.x, y+o.y);}
	Coord2D			operator-	(const Coord2D& o) const {return Coord2D (x-o.x, y-o.y);}
	Coord2D			operator-	() const {return Coord2D (-x, -y);}
	Coord2D			operator*	(const Coord2D& o) const {return Coord2D(x*o.x, y*o.y);}
	Coord2D			operator*	(double m) const {return Coord2D (x*m, y*m);}
	Coord2D			operator/	(double d) const {return Coord2D (x/d, y/d);}
	void			copy		(const Coord2D& o) {x=o.x; y=o.y;}
	void			copy		(const Coord3D& o);
	void			moveTo		(float x1, float y1) {x=x1; y=y1;}
	void			moveBy		(float x1, float y1) {x+=x1; y+=y1;}
	void			operator>>	(OStream& o) const {o.printf("(%f,%f)", x, y);}

	/** Computes distance between this and the other point.
	 *
	 *  @see sqdist
	 **/
	float			dist		(const Coord2D& o) const {return sqrt(sqr(x-o.x)+sqr(y-o.y));}

	/** Computes squared distance between this and the other point. (faster)
	 *
	 *  @see dist
	 **/
	float			sqdist		(const Coord2D& o) const {return sqr(x-o.x)+sqr(y-o.y);}
};

typedef Coord2D Vector2D;

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                    ___                    |  __  ___                     //
//                   /   \                   | /  \ |  \                    //
//                   |      __   __  |/\  ---|  __/ |   |                   //
//                   |     /  \ /  \ |   (   |    \ |   |                   //
//                   \___/ \__/ \__/ |    ---| \__/ |__/                    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

/** Three-dimensional coordinates in float space.
 **/
class Coord3D : public Object {
  public:
	float	x, y, z;

					Coord3D		(float x0=0, float y0=0, float z0=0) : x(x0), y(y0), z(z0) {;}
					Coord3D		(const Coord2D& o) {copy(o);}

	Coord3D&		operator=	(const Coord3D& o) {copy(o); return *this;}
	void			operator+=	(const Coord3D& o) {x+=o.x; y+=o.y; z+=o.z;}
	Coord3D			operator/	(double d) const {return Coord3D (x/d, y/d, z/d);}
	Coord3D			operator*	(double m) const {return Coord3D (x*m, y*m, z*m);}
	Coord3D			operator-	(const Coord3D& o) const {return Coord3D (x-o.x, y-o.y, z-o.z);}
	Coord3D			operator-	() const {return Coord3D (-x, -y, -z);}
	void			copy		(const Coord3D& o) {x=o.x; y=o.y; z=o.z;}
	void			copy		(const Coord2D& o) {x=o.x; y=o.y;}
	void			moveTo		(float x1, float y1, float z1) {x=x1; y=y1; z=z1;}
	void			moveBy		(float x1, float y1, float z1) {x+=x1; y+=y1; z+=z1;}
	OStream&		operator>>	(OStream& o) const {o.printf("(%f,%f)", x, y, z); return o;}

	/** Computes distance between this and the other point.
	 *
	 *  @see sqdist
	 **/
	float			dist		(const Coord3D& o) const {return sqrt(sqr(x-o.x)+sqr(y-o.y)+sqr(z-o.z));}

	/** Computes squared distance between this and the other point.
	 *
	 *  @see dist
	 **/
	float			sqdist		(const Coord3D& o) const {return sqr(x-o.x)+sqr(y-o.y)+sqr(z-o.z);}
};

END_NAMESPACE;

#endif
