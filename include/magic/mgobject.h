///////////////////////////////////////////////////////////////////////////////
// © Copyright Marko Grönroos 1996, all rights reserved.                     //
// Omistusoikeus Marko Grönroos, 1996, kaikki oikeudet pidätetään.           //
//   Tämän tiedoston kopiointi, lukeminen ja käyttö ja erityisesti tämän     //
// ilmoitustekstin muuttaminen on kiellettyä ilman asianmukaista lupaa.      //
//                                                                           //
//                                             Marko Grönroos (magi@utu.fi)  //
///////////////////////////////////////////////////////////////////////////////

#ifndef _GOBJECT_H
#define _GOBJECT_H

#include "magic/mobject.h"
#include "magic/mcoord.h"

BEGIN_NAMESPACE (MagiC);

//- Moduuli sisältää kaikenlaisia kivoja grafiikkaobjekteja, kuten viivoja,
//- ympyröitä, nelikulmioita, jne. ja niiden käsittelyyn liittyviä apufunktioita.<BR>
//- Mukana ei kuitenkaan ole mitään varsinaisia ikkunaobjekteja, vaan nämä ovat
//- täysin puhtaan loogisia objekteja.

///////////////////////////////////////////////////////////////////////////////
// Logical graphic structures

// Nelikulmio (Rectangle).<BR>
// Oletetaan, että <I>{x0&lt;x1</I> && <I>y0&lt;y1}</I>, tai muutoin
// tapahtuu jotain hyvin omituista, kuten että nelikulmioon kuuluvat
// kaikki pisteet, jotka ovat ko. alueen ulkopuolella...
class Rect {
  protected:
	Coord2D	mCorner1;
	Coord2D	mCorner2;
  public:

					Rect		() : mCorner1(0,0), mCorner2(0,0) {;}
					Rect		(float x0, float y0, float x1, float y1) {
						mCorner1.moveTo (x0, y0);
						mCorner2.moveTo (x1, y1);
					}
					Rect		(const Coord2D& s, const Coord2D& e) : mCorner1(s), mCorner2(e) {
					}

	void			operator=	(const Rect& other);

	// Koon muutokset ja luvut

	// Asettaa kaikki tiedot kerralla
	void			set			(float x0, float y0, float x1, float y1) {
		mCorner1.moveTo (x0, y0);
		mCorner2.moveTo (x1, y1);
	}
	void			set			(const Coord2D& s, const Coord2D& e) {
		mCorner1.copy (s);
		mCorner2.copy (e);
	}

	// Hakee kaikki koordinaattitiedot kerralla
	void			get			(float& x0, float& y0, float& x1, float& y1) const;

	const Coord2D&	lowerLeft	() const {return mCorner1;}
	Coord2D&		lowerLeft	() {return mCorner1;}
	const Coord2D&	upperRight	() const {return mCorner2;}
	Coord2D&		upperRight	() {return mCorner2;}
	
	// Asettaa vasemman yläkulman sijainnin
	void			setPos		(float x, float y);

	// Asettaa koon
	void			setSize 	(float xsize, float ysize);

	// Lukee koon
	void			getSize		(float& xsize, float& ysize) const;

	float			width		() const {return fabs(mCorner2.x-mCorner1.x);}
	float			height		() const {return fabs(mCorner2.y-mCorner1.y);}
	
	// Siirtäminen

	// Siirretään annettuun alkupaikkaan
	void			moveTo		(const Coord2D& pos);

	// Siirretään halutun offsetin verran
	void			moveBy		(const Coord2D& offset);

	// Muut

	// Testaa, onko annettu koordinaatti nelikulmion sisäpuolella
	bool			isInside	(const Coord2D& pos) const {
		return (pos.x>=mCorner1.x && pos.x<=mCorner2.x &&
				pos.y>=mCorner1.y && pos.y<=mCorner2.y);
	}	

	// Testaa koskettaako nelikulmio toista
	bool			touches		(const Rect& other) const;
};

END_NAMESPACE;

#endif

