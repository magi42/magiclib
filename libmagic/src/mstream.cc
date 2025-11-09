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

#include "magic/mobject.h"
#include "magic/mstream.h"
#include "magic/mpararr.h"

BEGIN_NAMESPACE (MagiC);

#ifdef SunOS
#include <sys/varargs.h>
#else
#include <stdarg.h>
#endif


class IStreamRef : private IODevice {
  protected:
	inline void			addStream			() {IODevice::addStream();}
	inline void			removeStream		() {IODevice::removeStream();}
	inline int			streamCount			() const {return IODevice::streamCount();}

	friend class Stream;
};

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                       ----                                                //
//                      (      |       ___   ___                             //
//                       ---  -+- |/\ /   )  ___| |/|/|                      //
//                          )  |  |   |---  (   | | | |                      //
//                      ___/    \ |    \__   \__| | | |                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

Stream::Stream ()
{
	mpDevice	= NULL;
	mOwnDevice	= false;
	mFormatMode	= 0;
}

/*****************************************************************************/
/** Constructor from a reference to a device.
 *
 *  Because the device was given with a reference, the stream object
 *  does not take the ownership of the device object and therefore
 *  does not destroy it along with itself.
 *****************************************************************************/
Stream::Stream (IODevice& dev) {
	mpDevice	= &dev;
	mOwnDevice	= false;
	mFormatMode	= 0;
}

/*******************************************************************************
* Constructor from a pointer to a device.
*
* Because the device was given with a pointer, the stream object takes the
* ownership of the device object and destroys it along with itself.
*******************************************************************************/
Stream::Stream (IODevice* dev)
{
	mpDevice	= dev;
	mOwnDevice	= true;
	mFormatMode	= 0;
}

/*******************************************************************************
* Shorthand constructor for a buffer device.
*
* The IODevice object is handled internally. The stream object keeps the
* ownership of the device object and destroys it along with itself.
*******************************************************************************/
Stream::Stream (String& buffer)
{
	mpDevice	= new Buffer (buffer);
	mOwnDevice	= true;
	mFormatMode	= 0;
}

/*******************************************************************************
* Shorthand constructor for a file device.
*
* The IODevice object is handled internally. The stream object keeps the
* ownership of the device object and destroys it along with itself.
*******************************************************************************/
Stream::Stream (FILE* strm)
{
	mpDevice	= new File (strm);
	mOwnDevice	= true;
	mFormatMode	= 0;
}

/*******************************************************************************
* Copy constructor.
*******************************************************************************/
Stream::Stream (Stream& orig)
{
	mpDevice    = orig.mpDevice;
	if (mpDevice)
		static_cast <IStreamRef*> (mpDevice)->addStream ();

	mOwnDevice  = orig.mOwnDevice;
	mFormatMode = orig.mFormatMode;
	mEncoding   = orig.mEncoding;
	mFlags      = orig.mFlags;
}

/*******************************************************************************
* Destructor for the stream.
*
*  The associated device is destructed along with the stream, unless
*  passed with a reference.
*******************************************************************************/
Stream::~Stream ()
{
	if (mOwnDevice)
		if (mpDevice) {
			static_cast <IStreamRef*> (mpDevice)->removeStream ();
			if (static_cast <IStreamRef*> (mpDevice)->streamCount() <= 0)
				delete mpDevice;
		}
}

/** @fndef device()
 *  @brief Returns the device the stream is connected to.
 **/

/*******************************************************************************
* Sets the device for the stream.
*
*  @note The stream object takes ownership of the device, and
*  destructs it properly.
*******************************************************************************/
void Stream::setDevice (IODevice* dev /**< Device to give to the stream.*/)
{
	mpDevice = dev;
	mOwnDevice = true;
}

/*******************************************************************************
** Returns true if the stream is at end.
*******************************************************************************/
bool Stream::atEnd () const
{
	if (mpDevice)
		return mpDevice->atEnd ();
	else
		return true;
}

/*******************************************************************************
 * Copy operator
 *
 * Warning: Does not take ownership of the device to which the stream
 * is attached.
 ******************************************************************************/
void Stream::copy (const Stream& other)
{
	TRACELINE;
	if (mpDevice && mOwnDevice)
		delete mpDevice;
	
	mpDevice    = other.mpDevice;
	mOwnDevice  = false; // WARNING
	mFormatMode = other.mFormatMode;
	mEncoding   = other.mEncoding;
	mFlags      = other.mFlags;
}


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                    ___   ----                                             //
//                   |   | (      |       ___   ___                          //
//                   |   |  ---  -+- |/\ /   )  ___| |/|/|                   //
//                   |   |     )  |  |   |---  (   | | | |                   //
//                   `___´ ___/    \ |    \__   \__| | | |                   //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/*****************************************************************************^
* Construction from a FILE output stream.
*
*  Example:
*  @code
*		FILE* myfile = fopen ("myfile.ext", "w");
*      OStream mystream (out);		// Attach to opened stream.
*      OStream outstd (stdout);	// Attach to standard output stream.
*  @endcode
*
*  @note It is assumed that the FILE stream really is open.
*******************************************************************************/
OStream::OStream (FILE* strm /**< Already opened FILE stream to use. */)
		: Stream (strm)
{
	mAutoFlush = false;
}

/*******************************************************************************
* Copy constructor.
*******************************************************************************/
OStream::OStream (OStream& orig) : Stream (orig)
{
	mAutoFlush = orig.mAutoFlush;
}

/*******************************************************************************
* Opens a stream attached to the given string buffer.
*
*  Example:
*  @code
*		String myBuffer;
*		OStream(myBuffer) << "Put some text there " << 123 << 42.42 << '\n';
*		OStream(myBuffer, IO_Append) << " Some appended text";
*  @endcode
*******************************************************************************/
OStream::OStream (String& buffer,	/**< String buffer to use. */
				  int mode			/**< (Optional) opening mode. IO_Writable is always enabled, but IO_Append and IO_Truncate may be very useful. */)
		: Stream (new Buffer (buffer, mode | IO_Writable))
{
}

/*******************************************************************************
* Flush the stream.
*******************************************************************************/
void OStream::flush ()
{
	if (mpDevice)
		mpDevice->flush ();
}

/** Standard copy operation */
void OStream::copy (const OStream& other)
{
	Stream::copy (other);

	mAutoFlush = other.mAutoFlush;
}

/** Formatted printing to stream.
 *
 *  \note The OStream::printf is not very efficient as it prints using
 *  a memory buffer. Inheritors should implement it by using device
 *  vformat interface.
 **/
OStream& OStream::printf (const char* sformat, ...)
{
	va_list args;
	va_start (args, sformat);

	*this << vstrformat (sformat, args);
	
	va_end (args);

	return *this;
}



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                    ---  ----                                              //
//                     |  (      |       ___   ___                           //
//                     |   ---  -+- |/\ /   )  ___| |/|/|                    //
//                     |      )  |  |   |---  (   | | | |                    //
//                    _|_ ___/    \ |    \__   \__| | | |                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * Creates an input stream and attaches it to a device.
 *
 * If the device is not yet open, it is opened as IO_Readable.
 ******************************************************************************/
IStream::IStream (IODevice* dev)
		: Stream (dev)
{
	/* Open the file if it wasn't opened already. */
	if (!mpDevice->isOpen())
		mpDevice->open (IO_Readable);
}

/*****************************************************************************/
/** Open the stream from an associated filename.
 *
 *  An internally handled File:IODevice object will be created to handle
 *  reading from the file.
 *****************************************************************************/
//bool IStream::openload (const char* filename /**< Filename to open. */)
//{
//	NOT_IMPLEMENTED;
//	return false;
//}

END_NAMESPACE;
