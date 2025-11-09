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

#ifndef __MAGIC_MTEXTSTREAM_H__
#define __MAGIC_MTEXTSTREAM_H__

#include "magic/mstream.h"

BEGIN_NAMESPACE (MagiC);

class TextStream;
class TextIStream;
class TextOStream;

///////////////////////////////////////////////////////////////////////////////
//            -----                ----                                      //
//              |    ___       |  (      |       ___   ___                   //
//              |   /   ) \ / -+-  ---  -+- |/\ /   )  ___| |/|/|            //
//              |   |---   X   |      )  |  |   |---  (   | | | |            //
//              |    \__  / \   \ ___/    \ |    \__   \__| | | |            //
///////////////////////////////////////////////////////////////////////////////

/** Abstract baseclass for text streams. */
class TextStream {
  public:
						TextStream		()				{mFieldWidth=0; mFillChar=' '; mPrecision=6; mBase=10;}
	virtual				~TextStream		()				{}
	
	inline int			width			() const		{return mFieldWidth;}
	inline int			width			(int w)			{int oldw=mFieldWidth; mFieldWidth=w; return oldw;}
	inline int			fill			() const		{return mFillChar;}
	inline int			fill			(int f)			{int oldf=mFillChar; mFillChar=f; return oldf;}
	inline int			precision		() const		{return mPrecision;}
	inline int			precision		(int f)			{int oldf=mPrecision; mPrecision=f; return oldf;}
	inline int			base			() const		{return mBase;}
	inline int			base			(int b)			{int oldf=mBase; mBase=b; return oldf;}
	void				copy			(const TextStream& other);

	enum formattingFlags {
		skipws		=	0x00000001,			/**< Skip whitespace automatically. */
		left		=	0x00000002,			/**< Left alignment of numeric fields. */
		right		=	0x00000004,			/**< Right alignment of numeric fields. */
		internal	=	0x00000008,			/**< Padding should be placed between sign and value. */
		showbase	=	0x00000010,			/**< Base of the values should be denoted when possible. */
		uppercase	=	0x00000020,			/**< Use uppercase denotation of base. */
		showpos		=	0x00000040,			/**< Print sign also for positive values. */
		scientific	=	0x00000080,			/**< Use scientific number notation for floating-point values. */
		fixed		=	0x00000100,			/**< Use fixed-point notation for floating-point values. */
	};
	
  private:
	int					mFieldWidth;	/**< Field width. */
	char				mFillChar;		/**< Field fill character. */
	int					mPrecision;		/**< Number of digits. */
	int					mBase;			/**< Base for printing values. */
};



///////////////////////////////////////////////////////////////////////////////
//         -----                ___   ----                                   //
//           |    ___       |  |   | (      |       ___   ___                //
//           |   /   ) \ / -+- |   |  ---  -+- |/\ /   )  ___| |/|/|         //
//           |   |---   X   |  |   |     )  |  |   |---  (   | | | |         //
//           |    \__  / \   \ `___´ ___/    \ |    \__   \__| | | |         //
///////////////////////////////////////////////////////////////////////////////

/** Text output stream.
 **/
class TextOStream : public OStream, public TextStream {
  public:
							TextOStream		(IODevice& dev) : OStream (dev) {;}
							TextOStream		(IODevice* dev) : OStream (dev) {;}
							TextOStream		(String& buffer, int mode = IO_Writable);
							TextOStream		(FILE* strm = stdout);
							TextOStream		(TextOStream& o);

	void					operator=		(const TextOStream& other) {copy (other);}
	void					copy			(const TextOStream& other);
	
	
	// Virtual methods
	
	virtual TextOStream&	printf			(const char* format, ...);
	virtual TextOStream&	operator<<		(const char* str);
	virtual TextOStream&	operator<<		(const String& str);
	virtual TextOStream&	operator<<		(const char);
	virtual TextOStream&	operator<<		(const int);
	virtual TextOStream&	operator<<		(const long);
	virtual TextOStream&	operator<<		(const float);
	virtual TextOStream&	operator<<		(const double);
	virtual uint			writeRawBytes	(const char* p, uint n);
};

// Standard output and atandard error output
extern TextOStream sout;
extern TextOStream serr;

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//          -----               ---  ----                                    //
//            |    ___       |   |  (      |       ___   ___                 //
//            |   /   ) \ / -+-  |   ---  -+- |/\ /   )  ___| |/|/|          //
//            |   |---   X   |   |      )  |  |   |---  (   | | | |          //
//            |    \__  / \   \ _|_ ___/    \ |    \__   \__| | | |          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/** Input stream. */
class TextIStream : public IStream, public TextStream {
  public:
							TextIStream		(IODevice& dev)			: IStream (dev) {;}
							TextIStream		(IODevice* dev)			: IStream (dev) {if (!dev->isOpen()) dev->open(IO_Readable);}
							TextIStream		(const String& buffer)	: IStream (const_cast<String&> (buffer)) {;}
							TextIStream		(FILE* strm = stdin)	: IStream (strm) {;}
							TextIStream		(TextIStream& o)		: IStream (o) {;}
							~TextIStream	() {}
	
	virtual TextIStream&	operator>>		(char& ch);
	virtual TextIStream&	operator>>		(int& value);
	virtual TextIStream&	operator>>		(long& value);
	virtual TextIStream&	operator>>		(float& value);
	virtual TextIStream&	operator>>		(double& value);
	virtual TextIStream&	operator>>		(String& buffer);
	virtual uint			readRawBytes	(char* data, uint maxlen);

	int						readAll			(String& textbuf);
	int						readLine		(String& linebuf);
	void					skipWhiteSpace	();

	TextIStream&			read			(char* p, int n);
};

extern TextIStream stin;

END_NAMESPACE;

#endif
