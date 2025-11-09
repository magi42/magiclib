/***************************************************************************
 *   This file is part of the MagiC++ library.                             *
 *                                                                         *
 *   Copyright (C) 1998-2005 Marko Grönroos <magi@iki.fi>                  *
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

#include <ctype.h>
#include <stdio.h>

#ifdef SunOS
#include <sys/varargs.h>
#else
#include <stdarg.h>
#endif

#include "magic/mobject.h"
#include "magic/mstring.h"
#include "magic/mclass.h"
#include "magic/mregexp.h"
#include "magic/mstream.h"
#include "magic/mtextstream.h"
#include "magic/mdatastream.h"
#include "magic/mexception.h"
#include "magic/mpararr.h"

BEGIN_NAMESPACE (MagiC);

impl_dynamic (String, {Comparable});
	

///////////////////////////////////////////////////////////////////////////////
//                         ----         o                                    //
//                        (      |          _                                //
//                         ---  -+- |/\ | |/ \   ___                         //
//                            )  |  |   | |   | (   \                        //
//                        ___/    \ |   | |   |  ---/                        //
//                                               __/                         //
///////////////////////////////////////////////////////////////////////////////

MagiC::String::String () {
	mLen	= 0;
	mMaxLen	= 0;
	mData	= NULL;
	mChkSum	= 0;
}

/** Copy constructor. */
MagiC::String::String (const String& orig	/**< String to copy. */)
{
	if (&orig == NULL || orig.mData==NULL) {
		mLen = 0;
		mMaxLen = 0;
		mData = NULL;
		return;
	}
	mLen = orig.mLen;
	mMaxLen = orig.mMaxLen;
	mData = new char [mMaxLen+1];
	memcpy (mData, orig.mData, mMaxLen+1);
	mChkSum	= 0;
}

/** Create from a NULL-terminated string. */
MagiC::String::String (const char* chrp		/**< NULL-terminated C-style string. */ )
{
	if (chrp) {
		mLen	= strlen (chrp);
		mMaxLen	= mLen;
		mData	= new char [mMaxLen+1];
		memcpy (mData, chrp, mLen+1);
	} else {
		mLen = mMaxLen = 0;
		mData = NULL;
	}
	mChkSum	= 0;
}

/** Create String from a NULL-terminated char buffer. */
MagiC::String::String (char* str,			 /**< NULL-terminated C-style string. */
					   enum strcrflags flags /**< Should the String object take the ownership of the character buffer? */)
{
	if (flags & STRCRFL_OWN)
		mData = str;
	else if (str)
		mData = strdup (str); // TODO
	else
		mData = NULL;

	mLen = mMaxLen = str? strlen (str) : 0;
	mChkSum = 0;
}

/** Create from a NULL-terminated string with given maximum length. */
MagiC::String::String (const char* chrp,	/**< NULL-terminated C-style string. */
					   int n				/**< Maximum (allocated) length of the buffer. There must be allocated one byte more than this, to store the end \x00.) */)
{
	if (chrp && n) {
		mLen	= n;
		mMaxLen	= mLen;
		mData	= new char [mMaxLen+1];
		memcpy (mData, chrp, mLen);
		mData [mMaxLen] = 0x00;
	} else {
		mLen = mMaxLen = 0;
		mData = NULL;
	}
	mChkSum	= 0;
}

/** Create String from a NULL-terminated char buffer. */
MagiC::String::String (char* str,			/**< The buffer must contain the terminating \x00 character. */
					   uint maxLen,			/**< Maximum (allocated) length of the buffer. There must be allocated one byte more than this, to store the end \x00. */
					   bool own				/**< Should the string object take ownership of the buffer? */)
{
	if (own)
		mData = str;
	else
		mData = strdup (str); // TODO

	mData[maxLen]	= '\0';
	mLen			= str? strlen (str) : 0;
	mMaxLen			= maxLen;
	mChkSum			= 0;
}

MagiC::String::~String () {
	delete mData;
	IFDEBUG (mData = (char*) 0xdddddddd);
}

template<class TYPE>
inline char* intToString(TYPE value, uint base) {
	const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	if (base > sizeof(digits))
		throw Exception (strformat ("Too large base %d for integer-to-String conversion", base));
	
	const int	maxintlen = 40;
	char		buf[maxintlen];
	char*		loc = buf + (maxintlen-1);
	*(loc--) = '\x00'; // Terminate

	bool isNeg = false; // Remember negative sign
	if (value < 0) {
		isNeg = true;
		value = -value;
	}

	do {
		*(loc--) = digits [value % base];
		value /= base;
	} while (value);

	if (isNeg) // Add sign if negative
		*(loc--) = '-';

	return strdup (loc+1);
}

/** Conversion from an integer */
MagiC::String::String (int i, int base) {
	mData = intToString<int> (i, base);
	mLen = mMaxLen = strlen(mData);
	mChkSum	= 0;
}

/** Conversion from an integer */
MagiC::String::String (uint i, int base) {
	mData = intToString<uint> (i, base);
	mLen = mMaxLen = strlen(mData);
	mChkSum	= 0;
}

/** Conversion from an integer */
MagiC::String::String (long i, int base) {
	mData = intToString<long> (i, base);
	mLen = mMaxLen = strlen(mData);
	mChkSum	= 0;
}

template <class TYPE>
inline char* formatFloat(TYPE f, char fmt, int prec)
{
	// Create formatting string
	char formatBuf [20];
	if (prec >= 0)
		sprintf (formatBuf, "%%.%d%c", prec, fmt);
	else
		sprintf (formatBuf, "%%%c", fmt);

	// Create actual string with the created format
	char buf [80+prec]; // Add prec, because it could be "high".
	sprintf (buf, formatBuf, f);

	return strdup (buf); // TODO: Use safedup
}

/** Conversion from a float */
MagiC::String::String (float f,		/**< Value to convert to string.			*/
					   char fmt,	/**< Formatting mode, either 'g' or 'f'.	*/
					   int prec		/**< Precision.								*/)
{
	mData = formatFloat<float> (f, fmt, prec);
	mLen = mMaxLen = strlen(mData);
	mChkSum	= 0;
}

/** Conversion from a double */
MagiC::String::String (double f,	/**< Value to convert to string.			*/
					   char fmt,	/**< Formatting mode, either 'g' or 'f'.	*/
					   int prec		/**< Precision.								*/)
{
	mData = formatFloat<double> (f, fmt, prec);
	mLen = mMaxLen = strlen(mData);
	mChkSum	= 0;
}

/** Assignment from a single character. */
String& MagiC::String::assign (char c)
{
	if (!mData)
		mData = new char [2];
	mData[0] = c;
	mData[1] = '\x00';
	mLen = 1;
	mChkSum = 0;
	return *this;
}

/** Concatenates a single character to the string.
 *
 *  This operation is O(1), as it uses the ensure_spontane() to grow the
 *  allocated the length of the string buffer.
 **/
String& MagiC::String::append (char c)
{
	// Ensure that our buffer is large enough
	ensure_spontane (mLen + 1);

	mData[mLen++]	= c;
	mData[mLen]		= '\x00'; // Terminate properly with \0
	mChkSum			= 0;

	return *this;
}

/** @fn String& MagiC::String::append (const String& other)
 *	@brief Concatenates another String to the string.
 **/

/** Concatenates another String to the string.
 **/
String& MagiC::String::append (const char* str,	/**< Char buffer, not necessarily \0-terminated. */
							   uint strlength	/**< Length of the buffer, not including a possible terminating\0. */)
{
	// Ensure that the string to append is not empty
	if (!str || strlength==0)
		return *this;
	
	// Ensure that our buffer is large enough
	if (mLen + int(strlength) > mMaxLen)
		ensure (mLen + strlength + 1);

	// Copy, but without a trailing \0
	memcpy (mData+mLen, str, strlength);

	mLen		+= strlength;
	mData[mLen]	=  '\x00'; // Terminate properly with \0
	mChkSum		=  0;

	return *this;
}

/** Replaces substring with a string of the same length.
 *
 *  If the replacement goes beyond the end of the string, the
 *  remainder is appended to the string.
 *
 *  Example:
 *  @code
 *		String ("Apple").replace (3, "eal", 3);  // Becomes "Appeal"
 *  @endcode
 **/
String& MagiC::String::replace (uint position,			/**< Start position for replacing. */
								const char* buffer,		/**< Buffer to replace with. */
								int buflen				/**< Length of the replacing buffer, or -1 if it is zero terminated. */)
{
	if (!buffer)
		return *this;
	
	if (buflen < 0)
		buflen = strlen (buffer);

	if ((int) position >= mLen)
		position = mLen;

	ensure_spontane (position + buflen);

	// Replace
	memcpy (mData+position, buffer, buflen);

	if ((int) (position + buflen) > mLen) {
		mLen = position + buflen;
		mData[mLen] = '\x00';
	}

	return *this;
}


/** Assignment from another String
 *
 *  @return Self.
 **/
String& MagiC::String::operator= (const String& other /**< String to copy with assignment. */)
{
	if (&other == NULL || other.mData==NULL || other.mLen==0) {
		// The string was null
		if (mData) {
			mLen	= 0;
			*mData	= '\x00'; // Preserve it
		}
	} else {
		// The string contains something
		mLen = other.mLen;
		if (mLen > mMaxLen) { // Reallocate only if necessary
			if (mData)
				delete mData;
			mMaxLen = mLen;
			mData = new char [mMaxLen+1];
		}
		memcpy (mData, other.mData, mLen+1);
	}
	mChkSum = other.mChkSum;
	return *this;
}

/*******************************************************************************
 * DESCRIPTION: Assignment from a null-terminated C-style string.
 *
 * RETURNS:     Self.
 ******************************************************************************/
String& MagiC::String::operator= (const char* chrp	/**< Zero-terminated char buffer. */)
{
	if (!chrp || !*chrp) {
		// The string was null or empty
		if (mData) {
			mLen	= 0;
			*mData	= '\x00'; // Preserve it
		}
	} else {
		// The string has something
		mLen = strlen (chrp);
		if (mLen > mMaxLen) { // Reallocate only if necessary
			if (mData)
				delete mData;
			mMaxLen = mLen;
			mData = new char [mMaxLen+1];
		}
		memcpy (mData, chrp, mLen+1);
	}
	mChkSum	= 0;
	return *this;
}

/*
ostream& MagiC::String::dump (ostream& out) const {
	out << format ("String #0x%x\n");
	out << "mData=" << mData << "\n";
	out << "mLen=" << mLen << "\n";
	out << "mMaxLen=" << mMaxLen << "\n";
	return out;
}
*/

/*******************************************************************************
 *  Compatibility operation for output to stream.
 *
 *  Prints the contents of the string to the stream. If the string is
 *  null (and not empty), it prints "(null)".
 *
 *  @note The string will be output as null-terminated. If the string
 *  was used as a binary buffer, its contents may be truncated.
 ******************************************************************************/
ostream& MagiC::String::operator>> (ostream& out) const
{
	if (mData)
		out << mData;
	else
		out << "(null)";
	return out;
}

/******************************************************************************/
/** Standard output operation to stream.
 *
 *  Prints the contents of the string to the stream. If the string is
 *  null (and not empty), it prints "(null)".
 *
 *  @note The string will be output as null-terminated. If the string
 *  was used as a binary buffer, its contents may be truncated.
 ******************************************************************************/
TextOStream& MagiC::String::operator>> (TextOStream& out) const
{
	if (mData)
		out << mData;
	else
		out << "(null)";
	return out;
}

/*******************************************************************************
 * Standard input operation from text stream.
 *
 * Reads one word from the stream, i.e., until next whitespace character.
 *
 * @note The string will be read as null-terminated. If the string was
 * used as a binary buffer, its contents may be truncated.
 ******************************************************************************/
TextIStream& MagiC::String::operator<< (TextIStream& is)
{
	ensure_spontane (80);
	mLen		= 0;
	mData[0]	= '\x00';

	int w = is.width (0);
	IODevice *dev = is.device (); // Read the device directly
	while (1) {
		// Read one character.
		int ch = dev->getch();

		//printf ("[%c]", ch);

		// Terminate string on either EOF or whitespace.
		if (ch == EOF)
			break;
		else if (isspace (ch))
			break;

		(*this) += ch;
		if (--w == 1)
			break;
	}

	if (mLen == 0) {
		// Didn't read anything. This is a problem. NOTE: REALLY????? Uh...
		//dev->setStatus (IO_ReadError);
	}

	return is;
}

/******************************************************************************/
/** Output to data stream
 ******************************************************************************/
DataOStream& MagiC::String::operator>> (DataOStream& arc) const
{
	arc.name ("mLen") << mLen;
	arc.name ("mMaxLen") << mMaxLen;
	arc.name ("mData").writeRawBytes (mData, mLen? mLen+1 : 0);
	return arc;
}

/******************************************************************************/
/** Input from data stream
 ******************************************************************************/
DataIStream& MagiC::String::operator<< (DataIStream& arc)
{
	arc >> mLen;
	arc >> mMaxLen;

	ensure (mMaxLen);
	arc.readRawBytes (mData, mLen+1);
	mData[mLen] = '\x00';
	mChkSum=0;
	
	return arc;
}

/******************************************************************************/
/** Concatenates String to the String and returns the result
 ******************************************************************************/
String MagiC::String::operator+ (const String& other) const
{
	String result = *this;
	result += other;
	return result;
}

/******************************************************************************/
/** Concatenates a char buffer to the string and returns the result
 ******************************************************************************/
String MagiC::String::operator+ (const char* str /**< Zero-terminated char buffer. */) const
{
	String result = *this;
	result += String(str);
	return result;
}

/******************************************************************************/
/** Returns true if value found.
 *  TODO: Make String private.
 ******************************************************************************/
bool findLowestNum (const String& str, uint& start, uint& end)
{
	// Scan the string using a state machine.
	enum states {NORMAL=0, FOUNDPERCENT};
	int state = NORMAL;		// Current state
	int number = 0; 		// Number currently being scanned
	int curPos = 0; 		// Starting position of the currently scanned number
	int lowest = 999999;	// Lowest number found so far
	start = 0;				// Start position of the lowest number found so far
	end = 0;				// End position of the lowest number found so far

	for (uint i=0; i<str.length(); i++) {
		switch (state) {
		  case NORMAL:
			  if (str[i] == '%') {
				  state = FOUNDPERCENT;
				  number = 0;
				  curPos = i;
			  }
			  break;
		  case FOUNDPERCENT:
			  if (isdigit(str[i]))
				  number = number*10 + int(str[i]-'0');
			  else {
				  if (number < lowest) {
					  // Found a new lowest
					  lowest	= number;
					  start		= curPos;
					  end		= i;
				  }
				  state = NORMAL;
			  }
			  break;
		}
	}
	if (state == FOUNDPERCENT && number < lowest) {
		// Found a new lowest in the end of the string
		start	= curPos;
		end		= str.length();
		lowest	= 0;
	}

	return (lowest!=999999);
}

/******************************************************************************/
/** Replaces the lowest occurrence of %n in the string.
 ******************************************************************************/
String MagiC::String::arg (const String& replacement, int fieldwidth) const {
	uint start, end;
	if (findLowestNum (*this, start, end)) {
		int		realwidth	= (replacement.length() > (uint)fieldwidth)? replacement.length() : fieldwidth;
		int		maxLen		= start + realwidth + (length()-end);
		char*	buf			= new char[maxLen + 1];
		char*	resultPos	= buf;

		// Concatenate beginning of the string, if not in the beginning
		if (start > 0) {
			memcpy (resultPos, mData, start);
			resultPos += start;
		}

		// Concatenate replacement (with padding)
		if (realwidth > 0) {
			for (int padding = (realwidth>fieldwidth)? 0 : fieldwidth-replacement.length(); padding>0; padding--)
				*(resultPos++) = ' ';
			memcpy (resultPos, replacement.mData, replacement.length());
			resultPos += replacement.length();
		}

		// Concatenate rest of the string, if not at end
		if (end < (uint) mLen) {
			memcpy (resultPos, mData+end, mLen-end);
			resultPos += mLen-end;
		}
		*resultPos = '\x00';

		// Create return object using the buffer.
		return String (buf, maxLen, true);
	} else
		// This is an error situation, basicly, but we return something nevertheless.
		return *this;
}

/** @fn String MagiC::String::arg (const char* str, int fieldwidth=0) const
 *
 *  Replaces the lowest occurrence of %n in the string.
 **/

/** @fn uint MagiC::String::length () const
 *
 *  @return Length of the string.
 **/

/******************************************************************************/
/** Deletes the last n characters from the string
 ******************************************************************************/
String& MagiC::String::dellast (uint n)
{
	if (!mData)
		return *this;

	// Delete at most the length of the string
	if (n >= (uint) mLen)
		n = mLen;
	mLen -= n;
	mData[mLen] = 0;
	mChkSum = 0;
	return *this;
}

/** Removes all trailing newline (\r, \n) characters. */
void MagiC::String::chop ()
{
	while (mLen>0 && isspace(mData[mLen-1]))
		dellast (1);
	mChkSum=0;
}

/** Fast comparison operator, based on cached checksum (hash
 *  value). Very quick if the hash values differ.
 *
 *  OBSERVE! This method doesn't calculate the checksums for the
 *  strings; you must calculate them manually!
 **/
int MagiC::String::fast_isequal (const String& other) const {
	if (other.mChkSum == mChkSum)
		return operator== (other.mData);
	else
		return 0;
}

/** @fn int MagiC::String::maxLength () const
 *
 *  Returns the current maximum length of the buffer.
 **/

/** Encodes the given string in two-character hex code. */
String& MagiC::String::hexcode (const String& other) {
	const char hexcodes[] = "0123456789abcdef";
	int otherlen = other.length();
	ensure (otherlen*2);
	for (int i=0; i<otherlen; i++) {
		*this += hexcodes [((unsigned int) other.mData[i])/16];
		*this += hexcodes [((unsigned int) other.mData[i])%16];
	}
	mChkSum=0;
	return *this;
}

/** Quote non-printable characters with given escape character.
 *
 *  @param quotechar Escape character for quotation, % by default.
 *  @param flags The default is QUOTE_NORMAL. QUOTE_HTML-flag
 *  quotes the spaces into +-characters.
 **/
void MagiC::String::quote (char quotechar, int flags) {
	FORBIDDEN; // Not implemented
}

/** Unquote non-printable characters with given escape character.
 *
 *  @param quotechar Escape character for quotation, % by default.
 *
 *  @param flags The default is QUOTE_NORMAL. QUOTE_HTML-flag
 *  quotes the spaces into +-characters.
 **/
void MagiC::String::unquote (char quotechar, int flags) {
	enum	states {Waiting, GotEqual, GotFirst, GotSoftLinebreak};

	String	hextable	= "0123456789ABCDEF";
	int		state		= Waiting;
	int		value		= 0;
	String	tmp;                             // Storage for characters after translation

	tmp.reserve (mLen);

	// Loop through string
	for (uint i=0 ; i < (uint) mLen ; i++) {
		char c = (*this)[i];
		switch (state) {
			case Waiting:
				if (c == quotechar)
					state=GotEqual;
				else
					if (c=='+' && (flags&QUOTE_HTML))
						tmp += ' ';
					else
						tmp += c;
				break;
				
			case GotEqual:
				if ((value=hextable.find(c)) != -1) {
					state=GotFirst;
					value*=16; // Shiftataan ylöspäin....
				} else { // Hmm, ei ollutkaan heksaluku...
					if (c=='\r') { // soft CRLF
						state = GotSoftLinebreak;
					} else {
						// Something weird...
						// Error in Q-P code! Let's recover it...
						state = Waiting;
						tmp += quotechar; 
						tmp += c;
					}
				}
				break;
				
			case GotFirst:
				if (hextable.find(c) != -1) {
					value+=hextable.find (c);
					tmp += char(value);
					state=Waiting; // Ok, palataan normaaliin tilaan...
				} else {
					// Jotain omituista, virhe QP-koodista.. yritetään toipua..
					tmp += quotechar;
					tmp += char(value);
					tmp += c;
					state = Waiting; // Så vi ventar igen..
				}
				break;
				
			case GotSoftLinebreak:
				// Not a soft linebreak after all?
				if (c!='\n')
					// Try to recover somehow...
					tmp += c;
				state=Waiting;
				break;
		};
	}

	if (state!= Waiting) {
		// Hmm, siellä oli siis lopussa yksinäinen '=' -> PANIC!
		tmp += '=';
		if (state==GotFirst)
			// DOUBLEPANIC!, mutta varaudutaan silti...
			// Hmm, tätä ei pitäisi tarvita tehdä...
			tmp += char(hextable[uint(state/16)]);
	} // DON'T PANIC!

	// Laitetaan loppunulli
	// tmp += '\x00';
	// tmp.mLen = respos;

	(*this) = tmp;
}

/** Returns a substring (0-based indexing). */
String MagiC::String::mid (uint from, int n) const {
	String result;
	
	// Jos pituus on -1, otetaan loput merkkijonosta
	if (n==-1)
		n = mLen-from;

	// Jos pyydetään merkkijonon ulkopuolelta tai tyhjä jono
	if (from >= (uint) mLen || mLen==0)
		return "";

	// Jos pyyntö menee merkkijonon ulkopuolelle, pienennetään pyyntöä
	if (((from + n) > (uint) mLen))
		n = mLen-from;

	ASSERT (mMaxLen>=n);
	
	// Luodaan uusi merkkijono
	result.mLen = n;
	result.mMaxLen = mMaxLen;
	result.mData = new char [mMaxLen+1];
	strncpy (result.mData, mData+from, n);
	result.mData [n] = 0;

	return result;
}

/** Returns new string that contains n leftmost letters of the string. */
String MagiC::String::left (uint n) const {
	return mid (0, n);
}

/** Returns new string that contains n rightmost letters of the string. */
String MagiC::String::right (uint n) const {
	String result;
	
	if (n < (uint) mLen)
		result = mid (mLen-n);
	else
		result = *this;

	return result;
}

/** Searches for a substring.
 *
 *  @return Position of the substring, or -1 of not found.
 **/
int MagiC::String::find (const String& subs, uint n) const {
	for (int i=n; i <= (mLen-subs.mLen); i++)
		if (!strncmp (mData+i, subs.mData, subs.mLen))
			return i;
	return -1;
}

/** Searches for a single character.
 *
 *  @return Position of the character, or -1 of not found.
 **/
int MagiC::String::find (const char c, uint n) const {
	char* pos = (char*) memchr (mData+n, c, mLen);
	return pos? pos-mData : -1;
}

/** Reverse substring search.
 *
 *  If start<0, it is calculated from the
 *  end of the string.
 *
 *  @return Position of the character, or -1 of not found.
 **/
int MagiC::String::findRev (const String& subs, int n) const {
	if (n<0)
		n += mLen;
	for (int i=n; i>=0; i--)
		if (!strncmp (mData+i, subs.mData, subs.mLen))
			return i;
	return -1;
}

String* MagiC::String::clone () const {
	return new String (*this);
}

/** Returns true if the string equals with the given char string. */
int MagiC::String::operator== (const char* other) const {
	if (!this)
		return 0;
	if (mData && other)
		return (!strcmp (mData, other));
	if ((!mData) && (!other))
		return 1;
	return 0;
}

/** Checks if the String equals another Comparable, which *MUST*
 *  be a string. This implements the Comparable::operator== that
 *  is used for sorting
 **/
int MagiC::String::operator== (const Comparable& other) const {
	if (getclass().issameclass (other))
		return operator== ((const String&) other);
	else
		return 0;
}

/** Reserves exactly the given length for the string (shortens the
 *  string if necessary).
 **/
void MagiC::String::reserve (int amount) {
	if (mData) {
		char* newmData = new char [amount+1];
		if (!newmData)
			throw runtime_error ("Out of memory or something in MagiC::String::reserve()");
		memcpy (newmData, mData, mLen+1);
		delete mData;
		mData = newmData;
		mData [amount] = 0x00;
	} else {
		mData = new char[amount+1];
		if (!mData)
			throw runtime_error ("Out of memory or something in MagiC::String::reserve()");
		*mData = 0x00;
	}
	mMaxLen = amount;
}

/** @fn void MagiC::String::ensure (int amount)
 *
 *  Ensures that at least the given length is reserved for the string.
 **/

/** @fn void MagiC::String::ensure_spontane (int amount)
 *
 *  Ensures that at least the given amount is reserved for the string,
 *  preferably a little (50%+4 characters) more.
 **/

/**	@fn void MagiC::String::grow_spontane ()
 *
 *  Grows the string somewhat (50%+4 characters)
 **/

/** Returns a const reference to the n:th character in the string. */
/*
const char& MagiC::String::operator[] (uint n) const {
	if (n<mLen)
		return mData [n];
	else
		throw Exception ("String index out of bounds");
}
*/

/** Returns a non-const reference to the n:th character in the string.*/
/*
char& MagiC::String::operator[] (uint n) {
	if (n<mLen)
		return mData [n];
	else {
		throw Exception ("String index out of bounds");
		return *mData; // To get rid of a warning
	}
}
*/

/*******************************************************************************
 * Splits the string into an array according to the given delimiter.
 ******************************************************************************/
void MagiC::String::split (Array<String>& trg, const char delim) const {
	if (!this || !mLen)
		return;

	// Use temporary copy
	char* tmp = new char [mLen+1];
	memcpy (tmp, mData, mLen+1);
	tmp[mLen]=0;

	// Change delimiters to null-characters
	int delimcount = 0;
	for (int i=0; i<mLen; i++)
		if (tmp[i]==delim) {
			tmp[i]=0;
			delimcount++;
		}

	// Create result array
	trg.make (delimcount+1);

	// Fill in response
	for (int pos = 0, i=0; pos<mLen; pos+=strlen (tmp+pos)+1, i++)
		trg.put (new String (tmp+pos), i);

	delete tmp;
}

/** Forms the string by joining the substrings in the given array with
 *  the given delimiter.
 **/
void MagiC::String::join (const Array<String>& src, const char delim) {
	// Lasketaan kohdemerkkijonon pituus tilanvarausta varten
	int newlen = src.size() - 1;
	for (int i=0; i<src.size(); i++)
		newlen += src[i].length();

	// Varataan tila
	reserve (newlen);
	*this = "";

	// Katenoidaan merkkijonot
	for (int i=0; i < src.size(); i++) 
		if (!src[i].isEmpty ()) {
			*this += src[i];
			if (i < src.size() - 1)
				*this += delim;
		}

	mChkSum=0;
}

/** Returns the string without any whitespace (space, tab, newline,
 *  cr, ff, vt) characters in the beginning or the end.
 **/
String MagiC::String::stripWhiteSpace () const {
	uint s=0, e=mLen;
	while (s < (uint) mLen && strchr (" \t\r\n",(*this)[s])) // TODO: Add ff, vt
		s++;
	while (e>s && strchr (" \t\r\n",(*this)[e-1])) // TODO: Add ff, vt
		e--;
	return mid (s, e-s);
}

/** Returns the string without any whitespace (space, tab, newline cr,
 *  ff, vt) characters in the beginning or the end, or any duplicates
 *  within the string.
 **/
String MagiC::String::simplifyWhiteSpace () const {
	if (mLen == 0)
		return String();

	char* 	buffer	= new char[mLen+1]; // Add 1 for terminating \x00
	char*	trgPos	= buffer;
	int		pos		= 0;

	// Strip whitespace from beginning
	for (pos=0; pos<mLen; pos++)
		if (!isspace(mData[pos]))
			break;

	// Strip duplicate whitespaces
	bool oneWhitespace=false;
	for (; pos<mLen; pos++) {
		if (isspace(mData[pos])) {
			if (!oneWhitespace)
				*(trgPos++) = mData[pos];
			oneWhitespace = true;
		} else {
			*(trgPos++) = mData[pos];
			oneWhitespace = false;
		}
	}

	// Strip whitespace from end (there can be only one)
	if (isspace (*(trgPos-1)))
		trgPos--;
	*trgPos = '\x00';

	if (trgPos > buffer)
		// Construct using the buffer.
		return String (buffer, mLen+1, true);
	else {
		// The string became empty -> return really empty string.
		delete buffer;
		return String ();
	}
}

void MagiC::String::upper() const {
	for (int i=0; i<mLen; i++)
		mData[i] = toupper (mData[i]);
}

void MagiC::String::lower() const {
	for (int i=0; i<mLen; i++)
		mData[i] = tolower (mData[i]);
}

/** Tries to match the given regular expression to the
 *  string.
 *
 *  @return true if the string matches.
 **/
int MagiC::String::regmatch (const char* expr) const {
	if (!mData)
		return 0;
	RegExp rexp (expr);
	return rexp.match (mData);
}

/** Tries to match the given regular expression to the string.
 *
 *  The second version supports parameters in the regular
 *  expression.
 *
 *  @param regexpr Regular expression.
 *
 *  @return != 0 if succesful and places the return values into
 *  the array.
 **/
int MagiC::String::regmatch (const char* expr, Array<String>& target) const {
	if (!mData)
		return 0;
	RegExp rexp (expr);
	return rexp.match (*this, target);
}

/** As above, but uses a precompiled regular expression.
 **/
int MagiC::String::regmatch (RegExp& compiled, Array<String>& target) const {
	if (!mData)
		return 0;
	return compiled.match (*this, target);
}

/** @fn bool MagiC::String::isNull () const
 *
 *  Is the string NULL?
 **/

/** @fn bool MagiC::String::isEmpty () const
 *
 *  Is the string empty (can be empty without being NULL)?
 **/

/** Empties the string.
 *
 *  Space reserved for the string buffer is not released, which makes
 *  emptying a fast operation. The string can then again used with no
 *  need for immediate buffer reallocation.
 **/
void MagiC::String::empty ()
{
	if (mData) {
		mLen = 0;
		mData[mLen] = '\x00';
	}
}

/** Calculates an 8-bit checksum for the string.
 **/
char MagiC::String::checksum () {
	mChkSum = 0;
	hashfunc (256);
	return mChkSum;
}

/** @fn char* MagiC::String::getbuffer () const
 *
 *  Returns a non-const pointer to the string buffer. Dangerous.
 **/

/** Computes hash value for the string.
 *
 *  Defined for the use of Comparable::hashfunc.
 *
 *  The hash function is simply h(c)=c%hashsize. Once the hash
 *  number has been calculated for the string, it is cached. This
 *  gives a considerable speedup for evaluating this method.
 **/
int	MagiC::String::hashfunc (int hashsize) const
{
	ASSERTWITH (hashsize<=256, format ("String hash size maximum is 256 slots",
									   " (hashsize was %d)", hashsize));
	
	// Jos on checksum laskettu, palautetaan sen mukainen arvo
	if (mChkSum)
		return mChkSum % hashsize;

	if (!mLen)
		return 0;

	int sum = 0;
	if (mLen < 10) {
		// Lyhyille merkkijonoille
		
		for (int i=0; i<mLen; i++)
			sum += mData[i];

		(unsigned char&) mChkSum = sum % 256;
	} else {
		// Pitemmille merkkijonoille

		// Sum of the first, middle and last character
		sum = int(mData[0]) + int(mData[mLen/2]) + mLen*int(mData[mLen-1]);

		(unsigned char&) mChkSum = sum % 256;
	}

	// TRACE4 ("%s -> %d/%d/%d", mData, sum, int(mChkSum), int(mChkSum%hashsize));
	return mChkSum % hashsize;
}

/** @fn char* MagiC::String::getbuffer () const
 *
 *  Returns a non-const pointer to the string buffer. Dangerous.
 **/

/*
MagiC::String::operator float () const {
	if (mData) {
		char* tmp = strdup (mData);
		for (char* tmp_p=tmp; *tmp_p; tmp_p++)
			if (*tmp_p == ',')
				*tmp_p = '.';
		float res = atof (tmp);
		free (tmp);
		return res;
	}
	return 0;
}
*/


///////////////////////////////////////////////////////////////////////////////////////////
//                                     |                              o                  //
//  ___       |   ___        _    ___  |     __         _    ___   |           _    ____ //
// /   ) \ / -+- /   ) |/\ |/ \   ___| |    /   |   | |/ \  |   \ -+- |  __  |/ \  (     //
// |---   X   |  |---  |   |   | (   | |    +-- |   | |   | |      |  | /  \ |   |  \__  //
//  \__  / \   \  \__  |   |   |  \__| |    |    \__! |   |  \__/   \ | \__/ |   | ____) //
//                                          |                                            //
///////////////////////////////////////////////////////////////////////////////////////////

/** A string that is always empty. */
const String /*MagiC::*/ emptystring;

/** @fn inline bool isempty (const String& str)
 *
 *  @return true if the string is empty or null.
 **/

/** Finds a character from a C-style string.
 *
 *  @return pointer to the location, or NULL if not found.
 **/
char* /*MagiC::*/ strnchr (const char* str, int mLen, char c) {
	const char* pos=str;
	for (int i=0; i<mLen; i++, pos++)
		if (*pos == c)
			return (char*) pos;
	return NULL;
}

/** Duplicates a null-terminated C-style string.
 *
 *  Safer version of strdup - checks if the string is NULL.
 *
 *  Also, uses new to allocate the string.
 *
 *  @return Pointer to the newly allocated copy.
 **/
char* safedup (const char* orig, int maxlen) {
	if (orig) {
		// Duplicate.
		// We can't use strdup, because ANSI C++ doesn't have it.
		int len = strlen (orig)+1; // Length with end-null.
		char* result = new char [len];
		memcpy (result, orig, len);
		return result;
	} else
		return (char*) NULL;
}

/*******************************************************************************
 * Reads one \n-terminated row from a stream to a String.
 *
 *  @return Number of read characters, or -1 in case of EOF.
 ******************************************************************************/
int /*MagiC::*/ fgetS (FILE* in, String& str) {
	char	cbuff [4097];
	char*   nlpos = NULL;
	int bytesread = 0;
	str = "";

	do {
		// Read a data block to buffer
		char* callResult = fgets (cbuff, 4096, in);

		if (callResult == NULL)
			break;

		// See if the block contains a newline
		nlpos = strnchr (cbuff, 4096, '\n');

		// Calculate the length of the block read
		int blocklength = nlpos? nlpos-cbuff+1 : 4096;

		// Add to result
		str.append (cbuff, blocklength);

		bytesread += blocklength;

		// Toistetaan kunnes löydetään lohkosta rivinvaihto
	} while (!nlpos && !feof (in));

	return bytesread>0? bytesread : -1;
}

/*******************************************************************************
 * Reads a string from the stream.
 *
 *  Terminated by given character, or with whitespace if -1.
 ******************************************************************************/
istream& /*MagiC::*/ getS	(istream& is, String& str, char term) {
	str = ""; // TODO: This may shorten the maxlen?
	str.ensure_spontane (80);
	
	register streambuf *sb = is.rdbuf ();
	bool eof=false;
	while (true) {
		int ch = sb->sbumpc ();
		if (ch == EOF) {
			eof = true;
			is.setstate (ios::eofbit);
			break;

			// Separation with given character, or whitespace, if
			// the terminator is \xff.
		} else if ((term!='\xff' && ch==term) || (term=='\xff' && isspace (ch)))
			break;
		
		// Skip CR
		else if (term!='\n' || ch!='\r')
			str += char(ch);
	}

	if (eof && str.length()==0)
		is.setstate (ios::failbit);

	return is;
}

void nullcharspace (int charspace [256]) {
	for (int i=0; i<256; i++)
		charspace [i]=0;
}

/** Loads a file to a String. */
void /*MagiC::*/ loadString (String& str, const String& filename) {
	FILE* in = fopen (filename, "rb");
	if (!in)
		return;
	str = "";
	String buff;
	buff.reserve (200);
	while (!feof(in)) {
		fgetS (in, buff);
		str += buff;
	}
	fclose (in);
}

/*****************************************************************************/
/** Prints to string with C-style formatting.
 *****************************************************************************/
String /*MagiC::*/ strformat (const char* sformat, /**< Formatting, as in C printf. */
							  ...)                 /**< Argument list as ellipsis.  */
{
	va_list args;
	va_start (args, sformat);
	String result = vstrformat (sformat, args);
	va_end (args);

	return result;
}

/*****************************************************************************/
/** Prints to string with C-style formatting.
 *****************************************************************************/
String /*MagiC::*/ vstrformat (const char* sformat, /**< Formatting, as in C printf. */
							   va_list ap)          /**< Argument list.              */
{
	va_list args;

	// Reserve a smallish buffer
	char* buffer = new char [STRFORMAT_SMALL_BUFFER_SIZE + 1];

	// Try writing to a smallish buffer.
	// The vsnprintf() modifies the va_list, and we have to end it.
	va_copy (args, ap);
	int written = vsnprintf (buffer, STRFORMAT_SMALL_BUFFER_SIZE + 1, sformat, args);
	va_end (args);

	// Check if it was enough
	if (written > STRFORMAT_SMALL_BUFFER_SIZE) {
		// Grow the buffer to the required size
		delete buffer;
		buffer = new char [written+1];

		// Reprint to the larger buffer.
		// The args has become invalid, so we have to reset it.
		va_copy (args, ap);
		vsnprintf (buffer, written+1, sformat, args);
		va_end (args);
	}

	return String (buffer, String::STRCRFL_OWN);
}

END_NAMESPACE;
