/***************************************************************************
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

#ifndef __MAGIC_IODEVICE_H__
#define __MAGIC_IODEVICE_H__

#include <magic/mobject.h>
#include <magic/mlist.h>
#include <magic/mexception.h>


// I/O Modes

BEGIN_NAMESPACE (MagiC);

#ifdef IO_Raw
#undef IO_Raw
#undef IO_Readable
#undef IO_ReadWrite
#undef IO_Truncate
#undef IO_Open
#undef IO_Append
#undef IO_Ok
#undef IO_ReadError
#undef IO_WriteError
#undef IO_FatalError
#undef IO_OpenError
#undef IO_ConnectError
#undef IO_AbortError
#undef IO_TimeOutError
#undef IO_OnCloseError
#endif

enum io_modes {
	IO_Raw			= 0x00000001,	/**< Raw (unbuffered) I/O-mode. */
	IO_Readable		= 0x00000002,	/**< I/O-mode for opening the device for reading. */
	IO_Writable		= 0x00000004,	/**< I/O-mode for opening the device for writing. */
	IO_ReadWrite	= 0x00000006,	/**< I/O-mode for opening the device for both reading and writing. */
	IO_Append		= 0x00000008,	/**< I/O-mode that indicates that device opened for writing should be opened at the end of file. */
	IO_Truncate		= 0x00000010	/**< I/O-mode that indicates that device opened for writing should be truncated. */
};

// I/O Types

// I/O States
enum io_states {
	IO_Open			= 0x00000100,	/**< State flag that indicates that the device is open. */
	IO_EOS			= 0x00000200	/**< The device is at end-of-stream. */
};

// I/O Statuses
enum io_statuses {
	IO_Ok			= 0x00000000,	/**< Operation was successful. */
	IO_ReadError	= 0x00000001,	/**< Error while reading. */
	IO_WriteError	= 0x00000002,	/**< Error while writing. */
	IO_FatalError	= 0x00000003,	/**< Fatal error. */
	IO_OpenError	= 0x00000004,	/**< Error while opening the device. */
	IO_ConnectError	= 0x00000005,	/**< Error while connecting the device. */
	IO_AbortError	= 0x00000006,
	IO_TimeOutError	= 0x00000007,	/**< Time out. */
	IO_OnCloseError	= 0x00000008,	/**< Error while closing the device. */
};


///////////////////////////////////////////////////////////////////////////////
//                 ---  ___  ___               o                             //
//                  |  |   | |  \   ___           ___   ___                  //
//                  |  |   | |   | /   ) |   | | |   \ /   )                 //
//                  |  |   | |   | |---   \ /  | |     |---                  //
//                 _|_ `___� |__/   \__    V   |  \__/  \__                  //
///////////////////////////////////////////////////////////////////////////////

/** Baseclass for I/O devices.
 **/
class IODevice : public Object {
  public:
						IODevice			();
	virtual 			~IODevice			();
	int					flags				() const {return mFlags;}
	int					mode				() const {return mMode;}

	bool				isDirectAccess		() const;
	bool				isSequentialAccess	() const;
	bool				isBuffered			() const {return !(mMode & IO_Raw);}
	bool				isRaw				() const {return mMode & IO_Raw;}
	bool				isSynchronous		() const;
	bool				isAsynchronous		() const;
	bool				isReadable			() const {return mMode & IO_Readable;}
	bool				isWritable			() const {return mMode & IO_Writable;}
	bool				isReadWrite			() const {return mMode & (IO_Readable | IO_Writable);}
	bool				isClosed			() const {return !(mState & IO_Open);}
	bool				isOpen				() const {return mState & IO_Open;}

	int					status				() const {return mStatus;}
	void				resetStatus			() {mStatus = IO_Ok;}
	int					state				() const {return mState;}
	virtual bool		open				(int mode);
	virtual void		close				();
	virtual void		flush				();
	virtual uint		size				() const;
	virtual int			at					() const;
	virtual bool		atEnd				() const;
	virtual bool		seek				(int);
	bool				reset				();

	virtual int			readBlock			(char* data, uint maxlen);
	virtual int			readLine			(char* data, uint maxlen);
	Ref<String>			readAll				();
	List<String>*		readLines			();
	virtual int			writeBlock			(const char* data, uint len);
	int					writeBlock			(const String&);
	virtual int			getch				();
	virtual void		putch				(char ch);
	virtual void		ungetch				(char ch);

  protected:
	void				setFlags			(int flags) {mFlags = flags;}
	void				setType				(int ftype) {mType = ftype;}
	void				setMode				(int fmode) {mMode = fmode;}
	void				setStatus			(int fstatus) {mStatus = fstatus;}
	void				setState			(int fstate, bool on=true) {if (on) mState |= fstate; else mState &= ~fstate;}

	void				setOpen				() {mState |= IO_Open;}
	void				setClosed			() {mState &= ~IO_Open;}

  protected:
	void				addStream			() {mStreams++;}
	void				removeStream		() {mStreams--;}
	int					streamCount			() const {return mStreams;}

  private:
	int		mFlags;
	int		mType;
	int		mMode;
	int		mState;
	int		mStatus;
	int     mStreams;
};



///////////////////////////////////////////////////////////////////////////////
//                              ----- o |                                    //
//                              |       |  ___                               //
//                              |---  | | /   )                              //
//                              |     | | |---                               //
//                              |     | |  \__                               //
///////////////////////////////////////////////////////////////////////////////

EXCEPTIONCLASS (file_already_open);

/** A file object for reading, writing, or otherwise manipulating files.
 *
 *  @example
 *  @code
 *     File myfile ("myfile.txt", IO_Readable);
 *     String linebuf;
 *     while (!in.atEnd()) {
 *         in.readLine (linebuf);
 *         sout << linebuf;
 *     }
 *  @endcode
 *
 *  For streamed interface, you can attach the file device to a
 *  stream.
 **/
class File : public IODevice {
  public:
						File		(FILE* str=stdout);
						File		(const String& name, int mode=0);

	const String&		name		() const {return mName;}
	void				setName		(const String& name);
	bool				exists		() const;
	bool				remove		();
	virtual bool		open		(int mode);
	virtual void		close		();
	virtual void		flush		();
	virtual uint		size		() const;
	virtual int			at			() const;
	virtual bool		seek		(int position);
	virtual bool		atEnd		() const;
	virtual	int			readBlock	(char* data, uint len);
	virtual int			readLine	(char* data, uint maxlen);
	int					readLine	(String& str, int maxlen=-1);
	virtual int			writeBlock	(const char* data, uint len);
	virtual int			getch		();
	virtual void		putch		(char);
	virtual void		ungetch		(char);
	int					handle		();

  private:
	FILE*	mpFile;		/**< File stream. */
	int		mFd;		/**< File descriptor. */
	String	mName;		/**< File name of the file. */

	static String		translateToFOpenMode	(int mode);
	static int			translateToOpenMode		(int mode);
};



///////////////////////////////////////////////////////////////////////////////
//                       ----                                                //
//                       |   )        __  __  ___                            //
//                       |---  |   | /   /   /   ) |/\                       //
//                       |   ) |   | +-- +-- |---  |                         //
//                       |___   \__! |   |    \__  |                         //
//                                   |   |                                   //
///////////////////////////////////////////////////////////////////////////////

/** A character buffer readable and writable with standard I/O methods.
 *
 *  @example
 *  @code
 *      String myString;
 *		Buffer myBuffer (myString);       // Attach buffer to a string (buffer)
 *      TextOStream myStream (myBuffer);  // Attach stream to the buffer
 *      myStream << "Hello, world!";      // Put something in the stream
 *      cout << myString << newl;         // Prints out "Hello, world!\n"
 *  @endcode
 **/
class Buffer : public IODevice {
  public:
						Buffer				(int mode = IO_ReadWrite);
						Buffer				(String& buffer, int mode = IO_ReadWrite);
						~Buffer				();

	virtual bool		open				(int mode);
	virtual uint		size				() const					{return mpBuffer? mpBuffer->length() : 0;}
	virtual int			at					() const					{return mPosition;}
	bool				reset				();
	virtual bool		atEnd				() const;
	virtual int			readBlock			(char* data, uint maxlen);
	virtual int			writeBlock			(const char* data, uint len);
	int					writeBlock			(const String&);
	virtual int			readLine			(char* data, uint maxlen);
	virtual int			getch				();
	virtual void		putch				(char ch);
	virtual void		ungetch				(char ch);
	const String&		getBuffer			() const {return *mpBuffer;}

  protected:
	String*	mpBuffer;
	bool	mOwnBuffer;
	int		mPosition;
};

END_NAMESPACE;

#endif
