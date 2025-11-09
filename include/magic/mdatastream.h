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

#ifndef __MAGIC_MDATASTREAM_H__
#define __MAGIC_MDATASTREAM_H__

#include "magic/mstream.h"

BEGIN_NAMESPACE (MagiC);

class DataStream;
class DataIStream;
class DataOStream;


///////////////////////////////////////////////////////////////////////////////
//           ___                    ----                                     //
//           |  \   ___   |   ___  (      |       ___   ___                  //
//           |   |  ___| -+-  ___|  ---  -+- |/\ /   )  ___| |/|/|           //
//           |   | (   |  |  (   |     )  |  |   |---  (   | | | |           //
//           |__/   \__|   \  \__| ___/    \ |    \__   \__| | | |           //
///////////////////////////////////////////////////////////////////////////////

/** Abstract baseclass for data streams. */
class DataStream {
  public:
						DataStream		() {}
	
  private:
};



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//        ___                    ___   ----                                  //
//        |  \   ___   |   ___  |   | (      |       ___   ___               //
//        |   |  ___| -+-  ___| |   |  ---  -+- |/\ /   )  ___| |/|/|        //
//        |   | (   |  |  (   | |   |     )  |  |   |---  (   | | | |        //
//        |__/   \__|   \  \__| `___´ ___/    \ |    \__   \__| | | |        //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

class DataOStream : public OStream, public DataStream {
  public:
	
						DataOStream		(FILE* strm=stdout);
						DataOStream		(OStream& o);
	virtual				~DataOStream	();


	// Implementations

	DataOStream&		operator<<		(char i);
	DataOStream&		operator<<		(int i);
	DataOStream&		operator<<		(long i);
	DataOStream&		operator<<		(float i);
	DataOStream&		operator<<		(double i);
	DataOStream&		operator<<		(const char* str);
	DataOStream&		operator<<		(const String& str);
	uint				writeRawBytes	(const char* p, uint n);

	/** Sets the name of the object <<:ed next to the stream.
	 *
	 *  \code out.name("mName") << mName;
	 **/
	DataOStream&		name		(const char* nam) {mNextName = nam; return *this;}

	/** Returns the name given with the name() function. */
	const String&		nextName	() const {return mNextName;}
	
	/** Returns the current indentation depth of the stream. */
	int				depth		() const {return mDepth;}

	const Array<String>&	getattribs	() const {return *mAttribs;}

  protected:

	DataOStream&		printName	();
	void				printIndent	();

	void				printComma	();
	
	friend DataOStream& operator<< (DataOStream& out, const Object& obj);
	friend DataOStream& operator>> (DataOStream& in, Object& obj);
	
	enum formattingFlags {
		FMT_RECORD			= 0x000002, /**< Dummy recording output mode for gathering attribute data. */
		FMT_BINARY			= 0x000000, /**< Should the stream be binary? */
		FMT_TEXT			= 0x000010, /**< Should the stream be text? */
		FMT_FORMATTED		= 0x000020, /**< Should the stream be formatted text? */
		FMT_VERINFO			= 0x000040, /**< Should the stream include version info? */
		FMT_ADDRESSES		= 0x000080, /**< Should the stream include memory addresses of the objects? */
		FMT_OBJECTNAMES		= 0x000100, /**< Should the stream include object names? */
		FMT_CLASSNAMES		= 0x000200, /**< Should the stream include class names of objects? */
	};

  protected:
	String			mNextName;	/**< Name of the next object to output, as given with the name() function. */
	int				mDepth;		/**< Indentation depth. */
	int				mPrevDepth;	/**< Indentation depth of previous output. */
	Array<String>*	mAttribs;	/**< Stored attributes. */
	int				mErrst;		/**< Error status. */
	
	int				open		(const char* filename, int flag);
};

// DataOStream& operator<< (DataOStream& dc, int i) {;}
// Sokerointioperaatio name()-liitteen tekemiseksi luokan attribuutteihin
// DataOStream& operator<< (DataOStream& out, const char* str);



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//         ___                   ---  ----                                   //
//         |  \   ___   |   ___   |  (      |       ___   ___                //
//         |   |  ___| -+-  ___|  |   ---  -+- |/\ /   )  ___| |/|/|         //
//         |   | (   |  |  (   |  |      )  |  |   |---  (   | | | |         //
//         |__/   \__|   \  \__| _|_ ___/    \ |    \__   \__| | | |         //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/** Input stream. */
class DataIStream : public IStream, public DataStream {
  public:
							DataIStream		(IODevice& dev)			: IStream (dev) {;}
							DataIStream		(IODevice* dev)			: IStream (dev) {;}
							DataIStream		(const String& buffer)	: IStream (const_cast<String&> (buffer)) {;}
							DataIStream		(FILE* strm = stdin)	: IStream (strm) {;}
							DataIStream		(DataIStream& o)		: IStream (o) {;}
	virtual					~DataIStream	() {}
	
	virtual DataIStream&	operator>>		(char& i);
	virtual DataIStream&	operator>>		(int& i);
	virtual DataIStream&	operator>>		(long& i);
	virtual DataIStream&	operator>>		(float& i);
	virtual DataIStream&	operator>>		(double& i);
	virtual DataIStream&	operator>>		(String& s);
	virtual uint			readRawBytes	(char* p, uint n);
};

END_NAMESPACE;

#endif
