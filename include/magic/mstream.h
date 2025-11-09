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

#ifndef __MAGIC_MSTREAM_H__
#define __MAGIC_MSTREAM_H__

#include "magic/mobject.h"
#include "magic/mstring.h"
#include "magic/miodevice.h"

BEGIN_NAMESPACE (MagiC);

class Stream;
class OStream;
class IStream;
class IOStream;

///////////////////////////////////////////////////////////////////////////////
//                       ----                                                //
//                      (      |       ___   ___                             //
//                       ---  -+- |/\ /   )  ___| |/|/|                      //
//                          )  |  |   |---  (   | | | |                      //
//                      ___/    \ |    \__   \__| | | |                      //
///////////////////////////////////////////////////////////////////////////////

/** Abstract streams baseclass.
 *
 *  All streams always have an associated device, the type of which can vary.
 **/
class Stream : public Object {
  public:
						Stream			();
						Stream			(IODevice& dev);
						Stream			(IODevice* dev);
						Stream			(String& buffer);
						Stream			(FILE* strm);
						Stream			(Stream& orig);
	virtual				~Stream			();

	/** Defines the character encoding of the stream. */
	enum Encoding {Locale=0,
				   Latin1,
				   Unicode,
				   UnicodeNetworkOrder,
				   UnicodeReverse,
				   RawUnicode,
				   UnicodeUTF8
	};

	inline IODevice*	device			()				{return mpDevice;}
	void				setDevice		(IODevice* dev);
	inline void			setEncoding		(Encoding enc)	{mEncoding = enc;}
	inline int			formatMode		() const		{return mFormatMode;}
	inline ulong		flags			() const		{return mFlags;}
	inline ulong		setFlags		(ulong f)		{int oldf=mFlags; mFlags=f; return oldf;}
	inline bool			operator[]		(ulong i)		{return flags() & i;}
	void				operator=		(const Stream& other) {copy (other);}
	virtual void		copy			(const Stream& other);

	bool				atEnd			() const;

  protected:
	IODevice*			mpDevice;
	bool				mOwnDevice;		/**< Does the object own the IODevice?	*/
	int					mFormatMode;	/**< Formatting mode.					*/
	Encoding			mEncoding;		/**< The text encoding mode.			*/
	ulong				mFlags;			/**< User-defined stream flags.			*/
};



///////////////////////////////////////////////////////////////////////////////
//                    ___   ----                                             //
//                   |   | (      |       ___   ___                          //
//                   |   |  ---  -+- |/\ /   )  ___| |/|/|                   //
//                   |   |     )  |  |   |---  (   | | | |                   //
//                   `___´ ___/    \ |    \__   \__| | | |                   //
///////////////////////////////////////////////////////////////////////////////

/** Abstract baseclass for output streams. */
class OStream : public Stream {
  public:
						OStream			(IODevice& dev) : Stream (dev) {;}
						OStream			(IODevice* dev) : Stream (dev) {;}
						OStream			(String& buffer, int mode = IO_Writable);
						OStream			(FILE* strm = stdout);
						OStream			(OStream& o);
	virtual				~OStream		() {}

	void				flush			();
	void				autoFlush		(bool afl=true)	{mAutoFlush=afl;}
	bool				isAutoFlush		() const		{return mAutoFlush;}
	void				operator=		(const OStream& other) {copy (other);}
	virtual void		copy			(const OStream& other);
	
	virtual OStream&	printf			(const char* format, ...);
	virtual OStream&	operator<<		(char) = 0;
	virtual OStream&	operator<<		(int) = 0;
	virtual OStream&	operator<<		(long) = 0;
	virtual OStream&	operator<<		(float) = 0;
	virtual OStream&	operator<<		(double) = 0;
	virtual OStream&	operator<<		(const char* str) = 0;
	virtual OStream&	operator<<		(const String& str) = 0;
	virtual uint		writeRawBytes	(const char* p, uint n) = 0;

  private:
	bool			mAutoFlush;
};



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                    ---  ----                                              //
//                     |  (      |       ___   ___                           //
//                     |   ---  -+- |/\ /   )  ___| |/|/|                    //
//                     |      )  |  |   |---  (   | | | |                    //
//                    _|_ ___/    \ |    \__   \__| | | |                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/** Input stream. */
class IStream : public Stream {
  public:
						IStream			(IODevice& dev)			: Stream (dev) {;}
						IStream			(IODevice* dev);
						IStream			(const String& buffer)	: Stream (const_cast<String&> (buffer)) {;}
						IStream			(FILE* strm = stdin)	: Stream (strm) {;}
						IStream			(IStream& o)			: Stream (o) {;}
	virtual				~IStream		() {}
	
	virtual IStream&	operator>>		(char& i)=0;
	virtual IStream&	operator>>		(int& i)=0;
	virtual IStream&	operator>>		(long& i)=0;
	virtual IStream&	operator>>		(float& i)=0;
	virtual IStream&	operator>>		(double& i)=0;
	virtual IStream&	operator>>		(String& s)=0;
	virtual uint		readRawBytes	(char* data, uint n)=0;

						operator bool	() {return !atEnd();}
	bool				operator !		() {return atEnd();}

  private:
	bool			mUnused;
};



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                 ---  ___   ----                                           //
//                  |  |   | (      |       ___   ___                        //
//                  |  |   |  ---  -+- |/\ /   )  ___| |/|/|                 //
//                  |  |   |     )  |  |   |---  (   | | | |                 //
//                 _|_ `___´ ___/    \ |    \__   \__| | | |                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/** Input and output stream. */
class IOStream : public OStream, virtual public IStream {
  public:

						IOStream		(IODevice& dev) : OStream (dev) {;}
						IOStream		(IODevice* dev) : OStream (dev) {;}
						IOStream		(String& buffer) : OStream (buffer) {;}
						IOStream		(FILE* strm) : OStream (strm) {;}
						IOStream		(IOStream& o) : IStream(o), OStream (o) {;}
};

END_NAMESPACE;

#endif
