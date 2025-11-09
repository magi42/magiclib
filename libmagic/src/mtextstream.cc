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

#include "magic/mtextstream.h"

#include <ctype.h>
#include <math.h>
#include <stdarg.h>

BEGIN_NAMESPACE (MagiC);

TextOStream sout (stdout); // Standard output
TextOStream serr (stderr); // Standard error output
TextIStream stin (stdin);  // Standard input


/*****************************************************************************/
/** Checks that the given device is valid.
 *
 *  If the device is not valid, raises stream_failure exception.
 *****************************************************************************/
inline void checkDevice (const IODevice* dev)
{
	if (!dev)
		throw stream_failure (i18n("Operated stream not associated with a device."));
}

///////////////////////////////////////////////////////////////////////////////
//            -----                ----                                      //
//              |    ___       |  (      |       ___   ___                   //
//              |   /   ) \ / -+-  ---  -+- |/\ /   )  ___| |/|/|            //
//              |   |---   X   |      )  |  |   |---  (   | | | |            //
//              |    \__  / \   \ ___/    \ |    \__   \__| | | |            //
///////////////////////////////////////////////////////////////////////////////

/** @fn int TextStream::width () const
 *
 *  @brief Returns the current field width.
 **/

/** @fn int TextStream::width (int w)
 *
 *  @brief Sets the field width and returns the old one.
 **/

void TextStream::copy (const TextStream& other)
{
	mFieldWidth = other.mFieldWidth;
	mFillChar   = other.mFillChar;
	mPrecision  = other.mPrecision;
	mBase       = other.mBase;
}


///////////////////////////////////////////////////////////////////////////////
//         -----                ___   ----                                   //
//           |    ___       |  |   | (      |       ___   ___                //
//           |   /   ) \ / -+- |   |  ---  -+- |/\ /   )  ___| |/|/|         //
//           |   |---   X   |  |   |     )  |  |   |---  (   | | | |         //
//           |    \__  / \   \ `___´ ___/    \ |    \__   \__| | | |         //
///////////////////////////////////////////////////////////////////////////////

/*****************************************************************************/
/** Construction from a FILE output stream.
 *
 *  Example:
 *  @code
 *		FILE* myfile = fopen ("myfile.ext", "w");
 *      OStream mystream (out);		// Attach to opened stream.
 *      OStream outstd (stdout);	// Attach to standard output stream.
 *  @endcode
 *
 *  @note It is assumed that the FILE stream really is open.
 *****************************************************************************/
TextOStream::TextOStream (FILE* strm	/**< Already opened FILE stream to use. */)
		: OStream (strm)
{
}

/*****************************************************************************/
/** Copy constructor.
 *****************************************************************************/
TextOStream::TextOStream (TextOStream& orig) /**< Stream object to copy. */
		: OStream (orig)
{
}

/*****************************************************************************/
/** Opens a stream attached to the given string buffer.
 *
 *  Example:
 *  @code
 *		String myBuffer;
 *		OStream(myBuffer) << "Put some text there " << 123 << 42.42 << '\n';
 *		OStream(myBuffer, IO_Append) << " Some appended text";
 *  @endcode
 *****************************************************************************/
TextOStream::TextOStream (String& buffer, /**< Buffer to write to. */
						  int mode)
		: OStream (buffer, mode)
{
}

/** Print formatted string. */
TextOStream& TextOStream::printf (const char* sformat, ...)
{
	if (!mpDevice)
		return *this;

 	va_list args;
	va_start (args, sformat);

	// Sprint to a buffer first.
	String buffer = vstrformat (sformat, args);

	// Write the buffer to the device
	mpDevice->writeBlock (buffer);
	
	va_end (args);
	
	if (isAutoFlush())
      mpDevice->flush();

   return *this;
}

void TextOStream::copy (const TextOStream& other)
{
	OStream::copy (other);
	TextStream::copy (other);
}

/** Print zero-terminated character string. */
TextOStream& TextOStream::operator<< (const char* str)
{
	if (mpDevice)
		mpDevice->writeBlock (str, strlen (str));
	return *this;
}

TextOStream& TextOStream::operator<< (const char c)
{
	if (mpDevice)
		mpDevice->putch (c);
	return *this;
}

TextOStream& TextOStream::operator<< (int x)
{
	if (mpDevice)
		mpDevice->writeBlock (String(x));
	return *this;
}

TextOStream& TextOStream::operator<< (long x)
{
	if (mpDevice)
		mpDevice->writeBlock (String(x));
	return *this;
}

TextOStream& TextOStream::operator<< (float x)
{
	if (mpDevice)
		mpDevice->writeBlock (String(x));
	return *this;
}

TextOStream& TextOStream::operator<< (double x)
{
	if (mpDevice)
		mpDevice->writeBlock (String(x));
	return *this;
}

TextOStream& TextOStream::operator<< (const String& str)
{
	if (mpDevice)
		mpDevice->writeBlock (str);
	return *this;
}

uint TextOStream::writeRawBytes (const char* str, uint length)
{
	if (mpDevice)
		return mpDevice->writeBlock (str, length);

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//          -----               ---  ----                                    //
//            |    ___       |   |  (      |       ___   ___                 //
//            |   /   ) \ / -+-  |   ---  -+- |/\ /   )  ___| |/|/|          //
//            |   |---   X   |   |      )  |  |   |---  (   | | | |          //
//            |    \__  / \   \ _|_ ___/    \ |    \__   \__| | | |          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/** Read character from stream. */
TextIStream& TextIStream::operator>> (char& rCh)
{
	checkDevice (mpDevice);

	// This is simple, just read the character
	rCh = mpDevice->getch ();

	return *this;
}

/** Read long value from stream. */
TextIStream& TextIStream::operator>> (long& rValue)
{
	/* This function is exactly same as above. Any way to remove the redundancy? */
	   
	checkDevice (mpDevice);

	// 	We need to read the value character by character.
	rValue = 0;
	int sign = 1; // Positive
	while (! mpDevice->atEnd()) {
		char ch = mpDevice->getch ();

		if (isdigit(ch))
			rValue = rValue*10 + long (ch - '0'); // The character is a digit; add it to the value.
		else if (ch == '-')
			sign = -1;
		else if (ch == '+')
			sign = 1;
		else {
			// It wasn't a digit, so we didn't want to read it.
			mpDevice->ungetch (ch);
			break;
		}
	}

	rValue *= sign;
	
	return *this;
}

/** Read integer value from stream. */
TextIStream& TextIStream::operator>> (int& rValue)
{

	// Read the value as double-precision
	long lvalue = 0;
	*this >> lvalue;

	// Convert it
	rValue = int (lvalue);
	
	return *this;
}

/** Read double-precision floating-point value from stream.
 *
 *  Skips any whitespace in front of the value, for example " 42" or "
 *  \t\n42". Does not skip any whitespace after the value.
 *
 *  The following regular expression defines the number format:
 *  [+-]?[0-9]+(\.[0-9]*)?([Ee][+-]?[0-9]+)?
 *
 *  For example, values such as "42", "-42", "+42", "+42.12345",
 *  "0.123", "1.5E10", "4.2E+10", and "4.2E-10" are valid.
 *
 *  Strings such as "42.", "42E" are not valid.
 **/
TextIStream& TextIStream::operator>> (double& rValue)
{
	checkDevice (mpDevice);

	// We parse the value with a state machine
	enum   states {NONE, SIGNFOUND, DOTFOUND, EXPFOUND, EXPSIGNFOUND, END} state = NONE;
	char   ch      = 0;     // Character read from device
	int    sign    = 1;     // Positive
	double fract   = 1.0;   // Multiplier for past-dot digits
	int    exp     = 0;     // Exponent
	int    expsign = 1;     // Sign of the exponent

	// Skip whitespace
	ch = mpDevice->getch ();
	while (isspace (ch))
		ch = mpDevice->getch ();
	
	// 	Read the value character by character, until value ends, or EOF.
	rValue = 0;
	while ((!mpDevice->atEnd()) && (state != END)) {
		// If the character is a digit, add it to the value
		switch (state) {
		  case NONE:
			  // Check for sign
			  if (state == NONE && ch == '-') {
				  sign = -1;
				  break;
			  } else if (state == NONE && ch == '+') {
				  sign = 1;
				  break;
			  }
			  state = SIGNFOUND; // It wasn't sign, so it might be a number.
			  // Fall through:
			  
		  case SIGNFOUND:
			  if (isdigit(ch)) {
				  rValue = rValue*10.0 + float (ch - '0');
			  } else if (ch == '.')
				  state = DOTFOUND;
			  else if (ch == 'e' || ch == 'E')
				  state = EXPFOUND;
			  else
				  state = END;
			  break;
			  
		  case DOTFOUND:
			  if (isdigit (ch)) {
				  // Handle post-comma digit
				  fract /= 10.0;
				  rValue = rValue + double (ch - '0')*fract;
			  } else if (ch == 'e' || ch == 'E')
				  state = EXPFOUND;
			  else
				  state = END;
			  break;
			  
		  case EXPFOUND:
			  // Check for sign
			  if (ch == '-') {
				  expsign = -1;
				  break;
			  } else if (ch == '+') {
				  expsign = 1;
				  break;
			  }
			  state = EXPSIGNFOUND; // It wasn't sign, so it might be a number.
			  // Fall through:
			  
		  case EXPSIGNFOUND:
			  if (isdigit(ch))
				  exp = exp*10 + int (ch - '0');
			  else
				  state = END;
			  break;

		  case END:
			  break;
		};

		// Read the next character
		ch = mpDevice->getch ();
	}

	if (state == END) {
		// It wasn't a digit, so we didn't want to read it.
		mpDevice->ungetch (ch);
	}
	
	// Correct the sign
	rValue *= sign;

	// Apply exponent
	if (exp != 0)
		rValue *= pow (10, double (exp*expsign));
	
	return *this;
}

/** Read floating-point value from stream.
 *
 *  Skips any whitespace in front of the value, for example " 42" or "
 *  \t\n42". Does not skip any whitespace after the value.
 *
 *  The following regular expression defines the number format:
 *  [+-]?[0-9]+(\.[0-9]*)?([Ee][+-]?[0-9]+)?
 *
 *  For example, values such as "42", "-42", "+42", "+42.12345",
 *  "0.123", "1.5E10", "4.2E+10", and "4.2E-10" are valid.
 *
 *  Strings such as "42.", "42E" are not valid.
 **/
TextIStream& TextIStream::operator>> (float& rValue)
{
	// Read the value as double-precision
	double dvalue = 0.0;
	*this >> dvalue;

	// Convert it
	rValue = float (dvalue);
	
	return *this;
}

/** Read a word from stream.
 *
 *  The word is delimited with a whitespace character. The function
 *  skips any whitespace character in front of the word. It does not,
 *  however, skip any whitespace after it.
 **/
TextIStream& TextIStream::operator>> (String& str)
{
	// Use the object's operation.
	//str.String::operator<< (*this);
	str.operator<< (*this);
	
	return *this;
}

/*****************************************************************************/
/** Reads a word from stream.
 *****************************************************************************/
TextIStream& TextIStream::read (char* p, /**< Buffer for reading.   */
								int n    /**< Length of the buffer. */)
{
	String buffer;
	buffer << *this;

	if ((buffer.length() > 0) && (n > 0))
		strncpy (p,
				 (CONSTR) buffer,
				 (uint(n) > buffer.length())? buffer.length() : n);
	
	return *this;
}

/*****************************************************************************/
/** Reads a raw byte block from stream.
 *****************************************************************************/
uint TextIStream::readRawBytes (char* data,
							   uint maxlen)
{
	if (mpDevice)
		return mpDevice->readBlock (data, maxlen);
	else
		return 0;
}

/*******************************************************************************
 * Reads line to given buffer.
 *
 * @return Number of bytes read (line length), or 0 if the file is at EOS
 * or not open.
 ******************************************************************************/
int TextIStream::readLine (String& linebuf)
{
	linebuf.empty ();

	if (!mpDevice || !mpDevice->isOpen() || mpDevice->atEnd())
		return 0;

	// Read line in chunks
	char buffer [1024+1];
	int bytesRead = 0;
	int totalRead = 0;
	while ((bytesRead = mpDevice->readLine (buffer, 1024))) {
		linebuf.append (buffer);
		totalRead += bytesRead;

		// If bytes read was 1024 bytes, there may be more.
		// WARNING: Should we check if the last byte is newline???
		if (bytesRead < 1024)
			break;
	}

	return totalRead;
}

END_NAMESPACE;

