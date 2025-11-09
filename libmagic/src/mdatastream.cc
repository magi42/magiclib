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

#include "magic/mdatastream.h"
#include "magic/mpararr.h"

BEGIN_NAMESPACE (MagiC);

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//        ___                    ___   ----                                  //
//        |  \   ___   |   ___  |   | (      |       ___   ___               //
//        |   |  ___| -+-  ___| |   |  ---  -+- |/\ /   )  ___| |/|/|        //
//        |   | (   |  |  (   | |   |     )  |  |   |---  (   | | | |        //
//        |__/   \__|   \  \__| `___´ ___/    \ |    \__   \__| | | |        //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/** @class FormatOStream
 *
 *  @brief Formatted output stream for structured storing of objects.
 *
 *  This class makes it possible to add formatting to serialization of
 *  object structures. Therefore, this class can be used for
 *  implementing XML serialization, and various other formats. By
 *  default, it outputs the objects using a structured format useful
 *  for debugging.
 *
 *  The @ref name() member function makes it possible to store also the
 *  names of the stored objects, in the context of the class that
 *  stores the objects.
 **/

DataOStream::DataOStream (FILE* strm) : OStream (strm) {
	mDepth		= 0;
	mPrevDepth	= -1;
	mFormatMode	= FMT_TEXT;
	mAttribs	= NULL;
}

DataOStream::DataOStream (OStream& o) : OStream (o) {
	mDepth		= 0;
	mPrevDepth	= -1;
	mFormatMode	= FMT_TEXT;
	mAttribs	= NULL;
}

DataOStream::~DataOStream () {
	delete mAttribs;
}

/** If the formatting mode is FMT_RECORD, we just store
 *  the attribute name and do not actually print anything.
 **/
#define CHECKRECORD(rtype,var) \
	if (mFormatMode & FMT_RECORD) {\
		if (mNextName.isEmpty())\
			mNextName = "?";\
		mAttribs->add (new String (strformat ("%s %s", #rtype, (CONSTR)mNextName)));\
		return *this;\
	}

DataOStream& DataOStream::operator<< (char i)
{
	CHECKRECORD (char, i);
	
	if (!mpDevice)
		return *this;

	if (mFormatMode & FMT_TEXT) {
		printComma ();
		printName ();
		mpDevice->writeBlock ((char*) &i, sizeof(char));
	} else { // FMT_BINARY
		mpDevice->writeBlock ((char*) &i, sizeof(i));
	}

	return *this;
}

DataOStream& DataOStream::operator<< (int i)
{
	CHECKRECORD (int, i);

	operator<< ((long) i);
	return *this;
}

DataOStream& DataOStream::operator<< (long i)
{
	CHECKRECORD (long, int (i));

	if (!mpDevice)
		return *this;

	if (mFormatMode & FMT_TEXT) {
		printComma ();
		printName ();
		mpDevice->writeBlock (String(i));
	} else { // FMT_BINARY
		mpDevice->writeBlock ((char*) &i, sizeof(i));
	}

	return *this;
}

DataOStream& DataOStream::operator<< (float i)
{
	CHECKRECORD (float, i);
	operator<< ((double) i);
	return *this;
}

DataOStream& DataOStream::operator<< (double i)
{
	CHECKRECORD (double, float(i));
	
	if (!mpDevice)
		return *this;

	if (mFormatMode & FMT_TEXT) {
		printComma ();
		printName ();
		mpDevice->writeBlock (String(i));
	} else { // FMT_BINARY
		mpDevice->writeBlock ((char*) &i, sizeof(i));
	}

	return *this;
}

DataOStream& DataOStream::operator<< (CONSTR str)
{
	CHECKRECORD (char*, p);

	if (!mpDevice)
		return *this;

	if (mFormatMode & FMT_TEXT) {
		printComma ();
		printName ();
	}

	mpDevice->writeBlock (str, strlen(str));
	return *this;
}

DataOStream& DataOStream::operator<< (const String& str)
{
	CHECKRECORD (char*, p);

	if (!mpDevice)
		return *this;

	return str.operator>> (*this);
}

uint DataOStream::writeRawBytes (const char* p, uint n)
{
	// TODO: CHECKRECORD (char*, p);
	
	if (!mpDevice)
		return 0;
	
	if (mFormatMode & FMT_TEXT) {
		printIndent ();
		printComma ();
		printName ();

		if (!p)
			return mpDevice->writeBlock ("(null)");
		else {
			String zeroterm (p, n);
			mpDevice->putch ('"');
			uint wrote = mpDevice->writeBlock (zeroterm);
			mpDevice->putch ('"');
			return wrote + 2;
		}
	} else // FMT_BINARY
		return mpDevice->writeBlock (p, n);
	
	return 0;
}

/** Prints the name stored with the name() function. */
DataOStream& DataOStream::printName ()
{ 
	if ((mFormatMode & FMT_TEXT) && !mNextName.isEmpty()) {
		mpDevice->writeBlock (mNextName);
		mpDevice->writeBlock ("=", 1);
		mNextName=""; // NULL originally, but causes ambiquity with some compilers
	}
	return *this;
}

/** Prints indentation, if desired in formatting. */
void DataOStream::printIndent ()
{
	if ((mFormatMode & FMT_TEXT) && false)
		for (int j=0; j<mDepth; j++)
			mpDevice->writeBlock ("    ", 4);
}

/** Prints comma if the indentation depth is same as with
 *  previous call.
 **/
void DataOStream::printComma ()
{
	if (mDepth <= mPrevDepth)
		mpDevice->writeBlock (", ", 2);
	mPrevDepth = mDepth;
}



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//         ___                   ---  ----                                   //
//         |  \   ___   |   ___   |  (      |       ___   ___                //
//         |   |  ___| -+-  ___|  |   ---  -+- |/\ /   )  ___| |/|/|         //
//         |   | (   |  |  (   |  |      )  |  |   |---  (   | | | |         //
//         |__/   \__|   \  \__| _|_ ___/    \ |    \__   \__| | | |         //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/** Read integer value from stream. */
DataIStream& DataIStream::operator>> (int& i)
{
	long i2=i;
	operator>> (i2);
	i=i2;
	return *this;
}

/** Read long value from stream. */
DataIStream& DataIStream::operator>> (long& i)
{
	int retval = mpDevice->readBlock ((char*) &i, sizeof(i));
	if (retval != sizeof(i))
		throw io_error ("Reading long value from text stream failed.");
	return *this;
}

/** Read character from stream. */
DataIStream& DataIStream::operator>> (char& i)
{
	int retval = mpDevice->readBlock ((char*) &i, sizeof(i));
	if (retval != sizeof(i))
		throw io_error ("Reading char value from text stream failed.");
	return *this;
}

/** Read float from stream. */
DataIStream& DataIStream::operator>> (float& i)
{
	int retval = mpDevice->readBlock ((char*) &i, sizeof(i));
	if (retval != sizeof(i))
		throw io_error ("Reading float value from text stream failed.");
	return *this;
}

/** Read double from stream. */
DataIStream& DataIStream::operator>> (double& i /**< Output reference. */)
{
	int retval = mpDevice->readBlock ((char*) &i, sizeof(i));
	if (retval != sizeof(i))
		throw io_error ("Reading double value from text stream failed.");
	return *this;
}

DataIStream& DataIStream::operator>> (String& s)
{
	/* Use the serialization provided by the string object. */
	s.String::operator<< (*this);
	
	return *this;
}

/** Read character buffer from stream. */
uint DataIStream::readRawBytes (char* p, /**< Buffer for reading.   */
								uint n    /**< Length of the buffer. */)
{
	if (!mpDevice)
		return 0;

	uint retval = mpDevice->readBlock (p, n);

	if (retval != n)
		throw io_error (String("Data stream ended unexpectedly (expected %1 bytes, got %2.")
						.arg((int) n).arg((int) retval));

	return retval;
}

////////////////////////////////////////////////////////////////////////////////
#if 0
DataIStream& operator<< (DataIStream& out,
						 const char* str)
{
	//out.name (str);
	//return out;
	return out;
}

/** Read integer value from stream. */
TextIStream& TextIStream::operator>> (int& i)
{
	long i2=i;
	operator>> (i2);
	i=i2;
	return *this;
}

/** Read long value from stream. */
TextIStream& TextIStream::operator>> (long& i)
{
	if (fread ((char*) &i, sizeof(i), 1, stream) != 1)
		throw IOError ();
	return *this;
}

/** Read character from stream. */
TextIStream& TextIStream::operator>> (char& i)
{
	if (fread ((char*) &i, sizeof(i), 1, stream) != 1)
		throw IOError ();
	return *this;
}

/** Read float from stream. */
TextIStream& TextIStream::operator>> (float& i)
{
	double i2=i;
	operator>> (i2);
	i=i2;
	return *this;
}

/** Read double from stream. */
TextIStream& TextIStream::operator>> (double& i /**< Output reference. */)
{
	if (fread ((char*) &i, sizeof(i), 1, stream) != 1)
		throw IOError ();
	return *this;
}

/** Read character buffer from stream. */
TextIStream& TextIStream::read (char* p, /**< Buffer for reading.   */
								int n    /**< Length of the buffer. */)
{
	int bytesread = fread (p, 1, n, stream);
	if (bytesread != n)
		throw IOError (i18n("%1 bytes read instead of %2 that were asked")
					   .arg(bytesread).arg(n));
	return *this;
}

TextIStream& TextIStream::operator>> (char* p)
{
	if (in)
		in->read (p, n);
	return *this;
}
#endif

END_NAMESPACE;
