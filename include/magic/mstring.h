/***************************************************************************
 *   @file mstring.h                                                       *
 *                                                                         *
 *   This file is part of the MagiC++ library.                             *
 *                                                                         *
 *   Copyright (C) 1998-2002 Marko Gr�nroos <magi@iki.fi>                  *
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

#ifndef __MAGIC_MSTRING_H__
#define __MAGIC_MSTRING_H__

#include <string.h> // fgetS and safedup require
#include <stdlib.h> // atoi() requires

#include "magic/mtypes.h"
#include "magic/mobject.h"
#include "magic/mmagisupp.h"

namespace MagiC {
	class TextOStream;
	class TextIStream;
	class DataOStream;
	class DataIStream;

	// Externals
	class RegExp;

	class String;
	class SubString;

	String	strformat	(const char* format, ...);
}

BEGIN_NAMESPACE (MagiC);

///////////////////////////////////////////////////////////////////////////////
//                         ----         o                                    //
//                        (      |          _                                //
//                         ---  -+- |/\ | |/ \   ___                         //
//                            )  |  |   | |   | (   \                        //
//                        ___/    \ |   | |   |  ---/                        //
//                                               __/                         //
///////////////////////////////////////////////////////////////////////////////

// Obey the global no-bounds-checking option
#ifdef NOCHECKBOUNDS
#define STRING_NOCHECKBOUNDS 1
#endif

/** Causes the String class NOT to check bounds anywhere.
 *  Checking makes world very safe, recommended.
 *  Can be a heavy bottleneck in some cases.
 **/
#ifdef STRING_NOCHECKBOUNDS // TODO!
#define STRING_GETCHAR(n) return mData[n];
#else
#define STRING_GETCHAR(n) return mData[n];
#endif

/** Generic string and buffer class.
 *
 *  Supports cached hash-number calculation.
 **/
class String : public Comparable {
	decl_dynamic (String);
  public:
	enum strcrflags {STRCRFL_NONE=0, STRCRFL_OWN=2};

					String				();
					String				(const String& orig);
					String				(const char*);
					String				(char* str, enum strcrflags flags);
					String				(const char*, int n);
					String				(char*, uint n, bool own);
	explicit		String				(int i, int base=10);
	explicit		String				(uint i, int base=10);
	explicit		String				(long i, int base=10);
	explicit		String				(float f, char fmt='g', int prec=6);
	explicit		String				(double f, char fmt='g', int prec=6);
	virtual			~String				();

	// Manipulation
	String&			assign				(char c);
	String&			append				(char c);
	String&			append				(const String& other) {return append (other.mData, other.mLen);}
	String&			append				(const char*, uint);
	String&			replace				(uint position, const char* buffer, int length=-1);

	// Operators
	String&			operator=			(const String&);
	String&			operator=			(const char*);
	String&			operator=			(char c) {return assign(c);}
	int				operator!=			(const char* b) const {return !operator== (b);}
	int				operator==			(const char*) const;
	int				operator==			(const String& b) const {return (*this) == b.mData;}
	int				operator==			(const Comparable& other) const;
	String&			operator+=			(const String& str) {return append (str.mData, str.mLen);}
	String&			operator+=			(char c) {return append (c);}
	String			operator+			(const String& str) const;
	String			operator+			(const char* str) const;
	const char		operator[]			(int n) const {STRING_GETCHAR(n)} // Varies on bounds checking, see above
	char&			operator[]			(int n) {STRING_GETCHAR(n)} // Varies on bounds checking, see above

	// Information
	bool			isNull				() const {if (this) return !mData; return 0;}
	void			nullify				();
	bool			isEmpty				() const {return (!this || !mData || !mLen);}
	void			empty				();
	uint			length				() const {return mLen;}
	int				maxLength			() const {return mMaxLen;}
	//void			truncate			(uint pos);
	//void			fill				(char c, int len=-1)

	// Formatting
	String			arg					(const String& str, int fieldwidth=0) const;
	String			arg					(const char* str, int fieldwidth=0) const {return arg (String (str), fieldwidth);}
	String			arg					(const char c, int fieldwidth=0) const {return arg (String (c), fieldwidth);}
	String			arg					(float x, int fieldwidth=0, char fmt='g', int prec=-1) const {return arg (String (x, fmt, prec), fieldwidth);}
	String			arg					(double x, int fieldwidth=0, char fmt='g', int prec=-1) const {return arg (String (x, fmt, prec), fieldwidth);}
	String			arg					(int x, int fieldwidth=0, int base=10) const {return arg (String (x, base), fieldwidth);}
	String			arg					(long x, int fieldwidth=0, int base=10) const {return arg (String (x, base), fieldwidth);}
	//String&		sprintf				(const char* format, ...);

	// Searching
	int				find				(const String&, uint start=0) const;
	int				find				(const char c, uint start=0) const;
	int				findRev				(const String&, int start=-1) const;
	int				regmatch			(const char* regexpr) const;
	int				regmatch			(const char* regexpr, Array<String>& target) const;
	int				regmatch			(RegExp& compiled, Array<String>& target) const;

	// Manipulations
	String			mid					(uint start, int len=-1) const;
	String			left				(uint n) const;
	String			right				(uint n) const;
	void			upper				() const;
	void			lower				() const;
	void			split				(Array<String>& target, const char delim=' ') const;
	void			join				(const Array<String>& source, const char delim=' ');
	String&			dellast				(uint n);
	void			chop				();
	String			stripWhiteSpace		() const;
	String			simplifyWhiteSpace	() const;

	// Allocation
	void			reserve				(int amount);
	void			ensure				(int amount) {if (mMaxLen<amount) reserve (amount);}
	void			ensure_spontane		(int amount) {if (mMaxLen<amount) reserve (amount+amount/2+4);}
	void			grow_spontane		() {reserve (mMaxLen+mMaxLen/2+4);}

	char			checksum			();
	int				fast_isequal		(const String& other) const;

	// Encodings
	String&			hexcode				(const String& other);
	enum			quoteflags			{QUOTE_NORMAL=0, QUOTE_HTML=1};
	void			quote				(char quotechar='%', int flags=0);
	void			unquote				(char quotechar='%', int flags=0);

	// I/O
	TextOStream&	operator>>			(TextOStream&) const;
	TextIStream&	operator<<			(TextIStream&);
	DataOStream&	operator>>			(DataOStream&) const;
	DataIStream&	operator<<			(DataIStream&);
	ostream&		operator>>			(ostream&) const;

	// Conversions
	int				toInt				() const {if (this && mData) return atoi (mData); return 0;}
	uint			toUInt				() const {if (this && mData) return (unsigned int) atoi (mData); return 0;}
	long			toLong				() const {if (this && mData) return atol (mData); return 0;}
	float			toFloat				() const {if (this && mData) return atof (mData); return 0;}
	double		toDouble			() const {if (this && mData) return atof (mData); return 0;}
	operator	const char*		() const {return (this != NULL)? mData : (const char*) NULL;}
	/** Returns a non-const pointer to the string buffer. Dangerous. */
	char*			getbuffer			() const {return mData;}

	// Implementations
	virtual String*	clone				() const;
	virtual int		hashfunc			(int hashsize) const;

  private:
	int				mLen;			/**< Current length of the string. */
	int				mMaxLen;		/**< Reserved length */
	char*			mData;
	unsigned char	mChkSum;		/**< Checksum calculated with the hash function. */

	friend String	MagiC::strformat	(const char* format, ...);
};



///////////////////////////////////////////////////////////////////////////////////////////
//                                     |                              o                  //
//  ___       |   ___        _    ___  |     __         _    ___   |           _    ____ //
// /   ) \ / -+- /   ) |/\ |/ \   ___| |    /   |   | |/ \  |   \ -+- |  __  |/ \  (     //
// |---   X   |  |---  |   |   | (   | |    +-- |   | |   | |      |  | /  \ |   |  \__  //
//  \__  / \   \  \__  |   |   |  \__| |    |    \__! |   |  \__/   \ | \__/ |   | ____) //
//                                          |                                            //
///////////////////////////////////////////////////////////////////////////////////////////


extern const String	emptystring;
inline bool		isempty		(const String& str) {return (!(&str) || str.isEmpty ());}
char*			strnchr		(const char* str, int len, char c);
char*			safedup		(const char* orig, int maxlen=-1);
int				fgetS		(FILE* in, String& str);
istream&		getS		(istream& in, String& str, char term='\n');
void			loadString	(String& str, const String& filename);
String			strformat	(const char* format, ...);
String			vstrformat	(const char* format, va_list ap);

#ifndef format
#define format strformat
#endif

/** Length of the default initial buffer used in strformat and vstrformat. */
#define STRFORMAT_SMALL_BUFFER_SIZE 80



///////////////////////////////////////////////////////////////////////////////
//                ----              ----         o                           //
//               (           |     (      |          _                       //
//                ---  |   | |---   ---  -+- |/\ | |/ \   ___                //
//                   ) |   | |   )     )  |  |   | |   | (   \               //
//               ___/   \__! |__/  ___/    \ |   | |   |  ---/               //
//                                                        __/                //
///////////////////////////////////////////////////////////////////////////////

// Virtual substring of a string.
class SubString : public String {
	String*		str;
	int			start, end;
  public:
};

END_NAMESPACE;

#include "magic/mi18n.h"

#endif
