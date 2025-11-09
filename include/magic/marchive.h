#ifndef __ARCHIVE_H__
#define __ARCHIVE_H__

#include <magic/mobject.h>
#include <magic/mstring.h>
#include <magic/mstream.h>

/*
///////////////////////////////////////////////////////////////////////////////
//
//								   CArchive
//

enum {	ARC_LOAD=0, ARC_SAVE=1, ARC_RECORD=2,
		ARC_BINARY=0, ARC_TEXT=16, ARC_FORMATTED=32,
		ARC_VERINFO=64,
		ARC_ADDRESSES=128};

// CArchive toteuttaa geneerisen objektien tallennuksen tiedostoon.
class CArchive {
	filebuf				fbuf;
	ostream*			out;
	istream*			in;
	String				arcfile;	// Tiedoston nimi
	int					mode;		// Tallennusmoodi
	String				nextname;	// Seuraavan objektin nimi
	int					indent;		// Sisennystaso
	Array<String>*	attribs;	// Muistiinpannut attribuutit
	
	int			open		(const char* filename, int flag);
  public:

	friend CArchive& operator<< (CArchive& out, const Object& obj);
	friend CArchive& operator>> (CArchive& in, Object& obj);
	int			errst;

				CArchive	()					{in=NULL; out=NULL; errst=0; indent=0; mode=0; attribs=NULL;}
	// Luo halutun nimisen archiven annetulla moodilla
	// (katso <I>liput</I> alla)
				CArchive	(const char* filnam, int md);
				~CArchive	();

	// Avaa arkiston tallennusta varten, palauttaa FALSE, jos
	// virhetilanne
	int			opensave	(const char* filename = NULL);

	// Avaa arkiston latausta varten, palauttaa FALSE, jos
	// virhetilanne
	int			openload	(const char* filename = NULL);

	int			openrecord	();
	
	void		close		();

	// Asettaa tekstiarkiston seuraavan objektin nimen
	CArchive&	name		(const char* nam)		{nextname=nam; return *this;}
	// Kirjoittaa 
	CArchive&	operator<<	(int i);
	CArchive&	operator<<	(long i);
	CArchive&	operator<<	(char i);
	CArchive&	operator<<	(float i);
	CArchive&	operator<<	(double i);
//	CArchive&	operator<<	(const char* i)			{if (out) out->write (i, strlen(i));  return *this;}

	// Kirjoittaa muistipuskurin sellaisenaan.
	// Tekstimuototilassa se heksakoodataan
	CArchive&	write		(const char* p, int n);

	// Lukee kokonaisluvun (todellisuudessa pitk‰n)
	CArchive&	operator>>	(int& i)			{long i2=i; operator>> (i2); i=i2; return *this;}
	// Lukee pitk‰n kokonaisluvun
	CArchive&	operator>>	(long& i);
	// Lukee yksitt‰isen merkin
	CArchive&	operator>>	(char& i);
	// Lukee liukuluvun (todellisuudessa pitk‰n)
	CArchive&	operator>>	(float& i)			{double i2=i; operator>> (i2); i=i2; return *this;}
	// Lukee pitk‰n liukuluvun
	CArchive&	operator>>	(double& i);
//	CArchive&	operator>>	(char* p)			{if (in) in->read (p, n); return *this;}
	// Lukee muistipuskurin
	CArchive&	read		(char* p, int n);

	const Array<String>&	getattribs	() const {return *attribs;}
};


//- <H3>Liput</H3>
//- <TABLE>
//-	<TR><TD><B> ARC_LOAD		<TD> Ladataan (oletus)
//-	<TR><TD><B> ARC_SAVE		<TD> Tallennetaan
//-	<TR><TD><B> ARC_RECORD		<TD> Laitetaan attribuuttien nimet vain muistiin
//-	<TR><TD><B> ARC_BINARY		<TD> Bin‰‰rimuotoinen arkisto (oletus)
//-	<TR><TD><B> ARC_TEXT		<TD> Tekstimuotoinen arkisto
//-	<TR><TD><B> ARC_FORMATTED	<TD> Objektihierarkian mukaan muotoiltu arkisto
//-	<TR><TD><B> ARC_VERINFO	<TD> Tallennetaanko alkuun versiotieto
//-	<TR><TD><B> ARC_ADDRESSES	<TD> Tallennetaanko muotoillussa muodossa 
//-									 mukaan objektien osoitteet
//- </TABLE><P>
//- N‰it‰ voi yhdistell‰ bin‰‰risell‰ | - operaatiolla

// Sokerointioperaatio name()-liitteen tekemiseksi luokan attribuutteihin
CArchive& operator<< (CArchive& out, const char* str);

*/

#endif
