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

#include <sys/types.h>	// For stat()
#include <sys/stat.h>	// For stat()
#include <unistd.h>		// For stat()
#include <fcntl.h>		// For open()
#include <errno.h>

#include "magic/miodevice.h"

BEGIN_NAMESPACE (MagiC);

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                 ---  ___  ___               o                             //
//                  |  |   | |  \   ___           ___   ___                  //
//                  |  |   | |   | /   ) |   | | |   \ /   )                 //
//                  |  |   | |   | |---   \ /  | |     |---                  //
//                 _|_ `___´ |__/   \__    V   |  \__/  \__                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/*****************************************************************************/
/** Default constructor for IODevice.
 **/
IODevice::IODevice ()
{
	mType = 0;
	mMode = 0;
	mState = 0;
	mStatus = 0;
}

/*******************************************************************************
 * Virtual destructor for IODevice.
 ******************************************************************************/
IODevice::~IODevice ()
{
}

/*******************************************************************************
 * Returns true if the device is capable of direct access.
 ******************************************************************************/
bool IODevice::isDirectAccess () const
{
	return false;
}

/*******************************************************************************
 * Returns true if the device is capable of sequential access.
 ******************************************************************************/
bool IODevice::isSequentialAccess () const
{
	return false;
}

/*******************************************************************************
 * Returns true if the device is synchronous, i.e., raw or unbuffered.
 ******************************************************************************/
bool IODevice::isSynchronous () const
{
	return mMode & IO_Raw;
}

/*******************************************************************************
 * Returns true if the device is asynchronous, i.e., buffered.
 ******************************************************************************/
bool IODevice::isAsynchronous () const
{
	return !(mMode & IO_Raw);
}

/*******************************************************************************
 * Opens device with given I/O mode.
 *
 *  Typical usage:
 *  @code
 *        File myFile ("/path/file.ext");
 *        myFile.open (IO_Readable);
 *        String myFileContents = myFile.readAll ();
 *        myFile.close ();
 *  @endcode
 *
 *  @li \c IO_Raw		Raw (unbuffered) I/O mode
 *  @li \c IO_Readable	The device is opened read-only
 *  @li \c IO_Writable	The device is opened write-only
 *  @li \c IO_ReadWrite	The device is opened read-write. This is equivalent to (IO_Readable | IO_Writable).
 *  @li \c IO_Append	The device is appended, if possible
 *  @li \c IO_Truncate	The device is truncated
 *
 *  @exception invalid_flags if the combination of mode flags was somehow invalid.
 *  @exception open_failure if there was some other error while opening.
 *
 *  @return true if successful, false if opening failed for an obvious reason.
 ******************************************************************************/
bool IODevice::open (int mode	/**< The opening mode is a or-ed combination of I/O mode flags. For example: (IO_Writable | IO_Truncate). */)
{
	MUST_OVERLOAD;
	return false;
}

/*******************************************************************************
 * Closes the device.
 *
 *  Flushes open buffers.
 ******************************************************************************/
void IODevice::close ()
{
	MUST_OVERLOAD;
}

/*******************************************************************************
 * Flushes (writes) the buffers.
 *
 *  Flushing ensures that all data that has been written to the device is
 *  really written, and does not wait in a buffer. This is important if an
 *  application crashes unexpectedly, or if we just want to be able to read
 *  it immediately.
 *
 *  This has only effect if the device has been opened for buffered
 *  writing.
 ******************************************************************************/
void IODevice::flush ()
{
	MUST_OVERLOAD;
}

/*******************************************************************************
 * Returns the size of the file in bytes.
 *
 *  Only meaningful for "block devices", which have a defined size.
 ******************************************************************************/
uint IODevice::size () const
{
	MUST_OVERLOAD;
	return 0;
}

/*******************************************************************************
 * Returns the current position in device.
 *
 *  Only meaningful for "block devices", which have a defined size.
 ******************************************************************************/
int IODevice::at () const
{
	MUST_OVERLOAD;
	return 0;
}

/*******************************************************************************
 * Returns true if currently at end of file, and false if not.
 *
 * This is useful for iterating over a device.
 ******************************************************************************/
bool IODevice::atEnd () const
{
	MUST_OVERLOAD;
	return true;
}

/*******************************************************************************
 * Changes the current position in device.
 *
 *  The function has meaning only if the device is capable of direct access.
 *
 *  @return true if successfull, false on failure.
 ******************************************************************************/
bool IODevice::seek (int position	/**< New position. */)
{
	return false;
}

/*******************************************************************************
 * Resets the state of the device to a newly opened state.
 *
 *  @return TRUE if successful, FALSE if an error occurred.
 ******************************************************************************/
bool IODevice::reset ()
{
	return true;
}

/*******************************************************************************
 * Reads a data block from the device.
 *
 *  @return Returns the number of bytes actually read, or 0 if none.
 ******************************************************************************/
int IODevice::readBlock (char* data,		/**< Data buffer           */
						 uint maxlen		/**< Length of the buffer. */ )
	throw (device_not_open)
{
	MUST_OVERLOAD;
	return 0;
}

/*******************************************************************************
 * Writes a data block to device.
 *
 *  @return Returns the number of bytes actually written, or 0 if none.
 ******************************************************************************/
int IODevice::writeBlock (
	const char* data,	/**< (output) Data buffer           */
	uint len)			/**< (input)  Length of the buffer. */
	throw (device_not_open)
{
	MUST_OVERLOAD;
	return 0;
}

/*******************************************************************************
 * Reads a newline (\\n) terminated line from device.
 *
 *  The buffer must be allocated to contain the given length of bytes.
 *
 *  The function spares one byte for the terminating zero, so the actual
 *  maximum number of bytes read is one less than the maxlen parameter.
 *
 *  Example:
 *  @code
 *  // Read in chunks
 *  char buffer [1024];
 *  int bytesRead = 0;
 *  int totalRead = 0;
 *  while (bytesRead = mpDevice->readLine (buffer, 1024)) {
 *      linebuf.append (buffer);
 *      totalRead += bytesRead;
 *  }
 *  @endcode
 *
 *  @note The default implementation reads the line with getch(). If
 *  you are implementing a device, you may want to implement a faster
 *  version that uses lower-level functions.
 *
 *  @return Returns the number of bytes actually read, or 0 if none.
 ******************************************************************************/
int IODevice::readLine (
	char* data,			/**< (output) Data buffer           */
	uint maxlen)		/**< (input)  Length of the buffer. */
	throw (device_not_open)
{
	int pos = 0; // Current position in the data buffer.

	// Read until the maximum length (minus 1 for the terminating zero) is
	// reached, or the end of file is reached.
	for (pos=0; !atEnd() && pos< int(maxlen)-1; pos++) {
		char ch = getch();
		data[pos] = ch;
		if (ch == '\n')
			break;
	}
	data[pos+1] = '\0'; // Terminate the string properly.

	return 0;
}

/*******************************************************************************
 * Writes a data block to device.
 *
 *  @return Returns the number of bytes actually written, or 0 if none.
 *****************************************************************************/
int IODevice::writeBlock (const String& str	/**< Data buffer to write. */ )
	throw (device_not_open)
{
	return writeBlock ((const char*) str, str.length());
}

/*******************************************************************************
 * Reads the entire device into a string buffer.
 *
 *  @return String buffer.
 *****************************************************************************/
Ref<String> IODevice::readAll ()
	throw (device_not_open)
{
	Ref<String> result = new String ();
	char buffer[1024];

	while (!atEnd()) {
		int bytesread = readBlock (buffer, 1024);
		if (bytesread == 0)
			break;
		result->append (buffer, bytesread);
	}
	
	return result;
}

/*******************************************************************************
 * Reads all newline (\\n) terminated lines from device.
 *
 *  @return Returns list of the lines, including the terminating
 *  newline characters.
 ******************************************************************************/
List<String>* IODevice::readLines ()
	throw (device_not_open)
{
	List<String>* result = new List<String> ();
	
	return result;
}

/*******************************************************************************
 * Reads and returns one character from device.
 *
 *  @return The character read.
 ******************************************************************************/
int IODevice::getch ()
	throw (device_not_open)
{
	MUST_OVERLOAD;
	return 0;
}

/*******************************************************************************
 * Writes one character to device.
 ******************************************************************************/
void IODevice::putch (char)
	throw (device_not_open)
{
	MUST_OVERLOAD;
}

/*****************************************************************************/
/** Unreads one character from device.
 *
 *  The function has no meaning if the device is already at beginning.
 *****************************************************************************/
void IODevice::ungetch (char)
	throw (device_not_open)
{
	MUST_OVERLOAD;
}



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                              ----- o |                                    //
//                              |       |  ___                               //
//                              |---  | | /   )                              //
//                              |     | | |---                               //
//                              |     | |  \__                               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * Standard constructor for File objects.
 *
 *  It is assumed that the FILE stream is already opened.
 ******************************************************************************/
File::File (FILE* file /**< An open FILE stream object to use. */)
		: IODevice ()
{
	mName		= "";
	mpFile		= file;
	mFd			= fileno (mpFile);

	// We assume that the FILE is already opened.
	setOpen ();
}

/*******************************************************************************
 * Constructor.
 *
 *  Creates a File object, and opens it if the mode parameter is given.
 *
 *  If the mode parameter is not given, the file can be opened later,
 *  or the object can be used to perform also other tasks on the file,
 *  such as deletion, etc.
 ******************************************************************************/
File::File (const String& name	/**< Name of the file to open. */,
			int mode			/**< Mode for opening. See @ref open for details. */)
	throw (open_failure)
		: IODevice ()
{
	mName	= name;
	mpFile	= NULL;
	mFd		= 0;

	if (mode) {
		// The device must be opened either readable or writable or both
		if (! ((mode & IO_Readable) || (mode & IO_Writable)))
			throw open_failure (i18n("File '%1' must be opened either readable or writable or both; mode was %2.")
								.arg(mName).arg(mode));
		
		// Open the file
		if (! open (mode))
			throw open_failure (i18n("Opening file '%1' with flags %2 failed for some reason.")
								.arg(mName).arg(mode));
		setOpen ();
	}
}

/*******************************************************************************
 * Returns true if the file exists.
 *
 *  @exception system_failure Thrown if there occurred some system
 *  problem while checking the existence of the file.
 ******************************************************************************/
bool File::exists () const throw (system_failure)
{
	struct stat statbuf; // For storing the status information about the file.

	// Get the file status
	int result = stat ((CONSTR) mName, &statbuf);

	if (result != 0) {
		// File does not exist
		if (errno == ENOENT)
			return false;

		// Some other error. Can't handle it cleanly.
		throw system_failure (i18n("System error '%1' while checking the existence of file '%2'.")
							  .arg(strerror(errno)).arg(mName));
	}
	
	return true; // File exists.
}

/*******************************************************************************
 * Removes a file or directory from the filesystem.
 *
 *  @return false if successful, true if failed for some reason.
 ******************************************************************************/
bool File::remove ()
{
	int result = ::remove ((CONSTR) mName);
	if (result != 0)
		return true;
	else
		return false;
}

/*******************************************************************************
 *
 ******************************************************************************/
bool File::open (int mode)
{
	if (isOpen())
		close ();

	// Translate mode to flags for the system open function.
	String fopenMode = translateToFOpenMode (mode);
	if (fopenMode.isEmpty())
		return false; // Failed
	
	// Try to open using the standard C open
	FILE* file = fopen ((CONSTR) mName, (CONSTR) fopenMode);

#ifdef IODEV_USE_EXCEPTIONS
	if (!file) {
		// Error state, errno has been set
		
		if (errno == EINVAL)
			 (i18n("Invalid mode flags '%1' for opening file '%2'.")
								 .arg(fopenMode).arg(mName));
		if (errno == ENOENT)
			throw file_not_found (i18n("File '%2' not found.")
								  .arg(mName));
			
		throw open_failure (i18n("Error '%1' while trying to open file '%2'.")
							.arg(strerror(errno)).arg(mName));
	}
#else
	if (!file)
		return false; // Failed
#endif

	// Set object state to open
	mpFile	= file;
	mFd		= fileno (mpFile);
	setMode (mode);
	setOpen ();

	return true; // Succeeded
}

/*******************************************************************************
 *
 ******************************************************************************/
void File::close ()
{
#ifdef IODEV_USE_EXCEPTIONS
	if (! mpFile)
		throw device_not_open (i18n ("File not open when closing file '%1'.").arg(mName));
#else
	if (! mpFile)
		return;
#endif

	// Close with the system call
	fclose (mpFile);

	// Set object state to closed.
	mpFile = NULL;
	mFd    = 0;
	setState (IO_Open, false);
}

/*******************************************************************************
 *
 ******************************************************************************/
void File::flush ()
{
	if (! mpFile)
		throw device_not_open (i18n ("File not open when flushing file '%1'.").arg(mName));

	fflush (mpFile);
}

/*******************************************************************************
 * Returns the size of the file.
 ******************************************************************************/
uint File::size () const
{
	struct stat statbuf; // For storing the status information about the file.

	// Get the file status
	int callResult = stat ((CONSTR) mName, &statbuf);

	if (!callResult)
		throw system_failure (i18n("System error '%1' while checking the size of file '%1'.")
							  .arg(strerror(errno)).arg(mName));

	return statbuf.st_size;
}

/*******************************************************************************
 *
 ******************************************************************************/
int File::at () const
{
	if (!isOpen())
		throw device_not_open (i18n ("File not open when inquiring the current position in file '%1'.").arg(mName));

	int position = ftell (mpFile);
	if (position < 0)
		throw system_failure (i18n("System error '%1' while inquiring the current position in file '%1'.")
							  .arg(strerror(errno)).arg(mName));

	return position;
}

/*******************************************************************************
 *
 ******************************************************************************/
bool File::seek (int position)
{
	if (!isOpen ())
		throw device_not_open (i18n ("File not open when inquiring the current position in file '%1'.").arg(mName));

	int callResult = fseek (mpFile, (long) position, SEEK_SET);

	if (!callResult)
		return false;

	return true;
}

/*******************************************************************************
 * Returns true if currently at end of file, and false if not.
 ******************************************************************************/
bool File::atEnd () const
{
	if (mpFile)
		return feof (mpFile);
	else
		return true;
}

/*******************************************************************************
 * Reads a data block of a given length to a buffer.
 *
 *  The buffer must be allocated to contain the given length of bytes.
 *
 *  @return The number of bytes actually read.
 ******************************************************************************/
int File::readBlock (char* data,	/**< Data buffer to receive the block read. */
					 uint maxlen	/**< Maximum number of bytes to read. */)
	throw (device_not_open)
{
	ASSERT (data && maxlen>0);
	if (atEnd())
		return 0;

	// TODO: Error handling
	int bytesread = fread (data, 1, maxlen, mpFile);

	if (feof(mpFile))
		setState (IO_EOS);

	return bytesread;
}

/*******************************************************************************
 *
 ******************************************************************************/
int File::readLine (char* data,		/**< Data buffer to receive the line. */
					uint maxlen		/**< Maximum number of bytes to read. */)
	throw (device_not_open)
{
	ASSERT (data && maxlen>0);
	if (atEnd())
		return 0;

	// TODO: Error handling
	char* datap = fgets (data, maxlen, mpFile);

	// WARNING: This does not discern between an error and a genuine EOF.
	if (!datap || feof(mpFile))
		setState (IO_EOS);

	return datap? strlen(datap) : 0;
}

/*******************************************************************************
 * Reads a newline-terminated line to a buffer.
 *
 *  The buffer is grown as needed, so the maxlen parameter can be
 *  much greater than the allocated length of the buffer.
 *
 *  @return Number of bytes actually read.
 ******************************************************************************/
int File::readLine (String& buffer,	/**< Data buffer to receive the line. */
					int maxlen		/**< Maximum number of bytes to read or -1 for unlimited. */)
	throw (device_not_open)
{
	if (atEnd())
		return 0;

	if (!isOpen())
		throw device_not_open (i18n ("File not open when reading a line from file '%1'.").arg(mName));

	// TODO: Error handling
	int bytesread = fgetS (mpFile, buffer);

	if (feof(mpFile))
		setState (IO_EOS);

	return bytesread;
}

/*******************************************************************************
 * Reads a block of data to file.
 *
 *  @return Number of bytes actually written.
 ******************************************************************************/
int File::writeBlock (const char* data,	/**< Data block to write. */
					  uint len			/**< Length of the data block. */)
	throw (device_not_open)
{
	if (!isOpen())
		throw device_not_open (i18n ("File not open when writing to file '%1'.").arg(mName));

	// TODO: Error handling
	int result = fwrite (data, 1, len, mpFile);

	return result;
}

/*******************************************************************************
 *
 ******************************************************************************/
int File::getch ()
	throw (device_not_open)
{
	if (!isOpen())
		throw device_not_open (i18n ("File not open when reading a from file '%1'.").arg(mName));

	// TODO: Error handling
	int result = ::fgetc(mpFile);
	
	if (result == EOF) {
		setState (IO_EOS);
		result = 0;
		return (-1);
	}
	
	return result;
}

/*******************************************************************************
 *
 ******************************************************************************/
void File::putch (char ch)
	throw (device_not_open)
{
	if (!isOpen())
		throw device_not_open (i18n ("File not open when writing to file '%1'.").arg(mName));

	// int status = 
	fputc (ch, mpFile);
	// TODO: Error handling
}

/*******************************************************************************
 * Ungets the last character read from the device.
 *
 * \note The file may be at EOF when this function is called. In such case,
 * the function returns the object to non-EOF state.
 ******************************************************************************/
void File::ungetch (char ch)
	throw (device_not_open)
{
	if (!isOpen())
		throw device_not_open (i18n ("File not open when ungetting from file '%1'.").arg(mName));
	
	// If at EOS, return to open state.
	if (state() & IO_EOS)
		setState (IO_EOS, false);

	// int status = 
	ungetc (ch, mpFile);
	// TODO: Error handling
}

/*******************************************************************************
 *
 ******************************************************************************/
int File::handle ()
{
	return mFd;
}

/*******************************************************************************
 * Translates IODevice mode flags to system mode flags for fopen().
 ******************************************************************************/
String File::translateToFOpenMode (int mode /**< IODevice mode flags to translate. */) throw (invalid_flags)
{
	if (mode & IO_Readable)
		if (mode & IO_Writable)
			if (mode & IO_Append)
				return "a+";			// Open for RW, do not truncate, position at EOF
			else
				if (mode & IO_Truncate)
					return "w+";		// Open for RW, truncate, position at BOF
				else
					return "r+";		// Open for RW, do not truncate, position at BOF
		else
			return "r";					// Open for R, position at BOF
	else
		if (mode & IO_Writable)
			if (mode & IO_Append)
				return "a";				// Open for W, do not truncate, position at EOF
			else
				return "w";				// Open for W, truncate, position at BOF

	return ""; // This would actually be an error state.
}

/*******************************************************************************
 * Translates IODevice mode flags to system mode flags for open().
 ******************************************************************************/
int File::translateToOpenMode (int mode /**< IODevice mode flags to translate. */) throw (invalid_flags)
{
	int sysmode = 0;
	if (mode & IO_Readable)
		if (mode & IO_Writable)
			sysmode |= O_RDWR;
		else
			sysmode |= O_RDONLY;
	else
		if (mode & IO_Writable)
			sysmode |= O_WRONLY;
		else
			return false; // Failed

	if (mode & IO_Append)
		sysmode |= O_APPEND;

	if (mode & IO_Truncate)
		sysmode |= O_TRUNC;

	return sysmode;
}



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                       ----                                                //
//                       |   )        __  __  ___                            //
//                       |---  |   | /   /   /   ) |/\                       //
//                       |   ) |   | +-- +-- |---  |                         //
//                       |___   \__! |   |    \__  |                         //
//                                   |   |                                   //
///////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * Default constructor for Buffer.
 ******************************************************************************/
Buffer::Buffer (int mode)
{
	mpBuffer	= new String();
	mPosition	= 0;
	mOwnBuffer	= true;

	// Try to open, if mode was given
	if (mode)
		if (! open (mode))
			throw open_failure (i18n("Opening Buffer device with flags %1 failed for some reason.")
								.arg(mode));
}

/*******************************************************************************
 * Constructs a buffer to use a given string as the buffer.
 *
 *  Notice that the string is _not_ destructed along with the Buffer.
 ******************************************************************************/
Buffer::Buffer (String& buffer, int mode)
{
	mpBuffer	= &buffer;
	mPosition	= 0;
	mOwnBuffer	= false;

	// Try to open, if mode was given
	if (mode)
		if (! open (mode))
			throw open_failure (i18n("Opening Buffer device with flags %1 failed for some reason.")
								.arg(mode));
}

Buffer::~Buffer ()
{
	if (mOwnBuffer)
		delete mpBuffer;
}

bool Buffer::open (int mode)
{
	setMode (mode);
	setOpen ();

	// This basicly resets position, etc.
	reset ();

	if (mode & IO_Truncate)
		mpBuffer->empty ();

	if (mode & IO_Append)
		mPosition = mpBuffer->length ();

	return true;
}

bool Buffer::reset ()
{
	if (!IODevice::reset ())
		return false;

	mPosition = 0;
	return true;
}

bool Buffer::atEnd () const
{
	if ((uint) mPosition >= size())
		return true;
	else
		return false;
}

int Buffer::readBlock (char* data, uint maxlen)
	throw (device_not_open)
{
	if ((uint) mPosition >= size())
		return 0;

	int remains = mpBuffer->length() - mPosition;			// How far is the end of buffer
	int readcount = ((uint) remains > maxlen)? maxlen : remains;	// How many bytes will actually be read

	memcpy (data, ((const char*) *mpBuffer) + mPosition, readcount);

	mPosition += readcount;
	return readcount;
}

int Buffer::writeBlock (const char* data, uint len)
	throw (device_not_open)
{
	mpBuffer->replace (mPosition, data, len); 
	mPosition += len;
	return len;
}

int Buffer::readLine (char* data, uint maxlen)
	throw (device_not_open)
{
	if ((uint) mPosition >= size())
		return 0;

	int  remains    = mpBuffer->length() - mPosition;			    // How far is the end of buffer
	int  maxread    = ((uint) remains > maxlen)? maxlen : remains;	// How many bytes can actually be read
	uint newlinePos = mPosition;

	// Find the next newline
	for (uint i=0; i < (uint) maxread; i++)
		if ((*mpBuffer)[newlinePos++] == '\n')
			break;

	// How much was actually read
	int readcount = newlinePos - mPosition;

	// Copy everything up to the newline to the result buffer.
	memcpy (data, ((const char*) *mpBuffer) + mPosition, newlinePos-mPosition);
	data[readcount] = '\x00';

	// Move position pointer to new position
	mPosition = newlinePos;

	return readcount;
}

int Buffer::writeBlock (const String& str)
	throw (device_not_open)
{
	mpBuffer->append (str);
	return str.length();
}

int Buffer::getch ()
	throw (device_not_open)
{
	if ((uint) mPosition >= size())
		return -1; // EOS

	// TODO: Should we increment?
	return (*mpBuffer)[uint(mPosition)];
}

void Buffer::putch (char c)
	throw (device_not_open)
{
	// Use reserved buffer if possible, otherwise append (grow the
	// buffer).
	if ((uint) mPosition >= size()) {
		// No room in buffer, append.
		mpBuffer->append (c);
		mPosition++;
	} else
		(*mpBuffer)[uint(mPosition++)] = c;
}

void Buffer::ungetch (char c)
	throw (device_not_open)
{
	if (mPosition==0)
		return;

	// Go back one character, and put the character there.
	(mpBuffer)[--mPosition] = c;
}

END_NAMESPACE;
