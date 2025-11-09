/***************************************************************************
 *   This file is part of the libMagiC++ library.                          *
 *                                                                         *
 *   Copyright (C) 2005 Marko Grönroos <magi@iki.fi>                       *
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

#include <magic/mlog.h>
#include <magic/mstring.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdexcept>
#include <time.h>

BEGIN_NAMESPACE (MagiC);

Log::Log ()
{
	mModuleDepth = 0;
}

Log::~Log ()
{
}

/*******************************************************************************
 * \fn int Log::open()=0
 *
 * Opens the log file.
 ******************************************************************************/

/*******************************************************************************
 * \fn void Log::close()=0
 *
 * Closes the log file.
 ******************************************************************************/

/*******************************************************************************
 * Writes a log message
 *
 * \par Example:
 * \code
 * 		mpLog->message ("SERVER",
 *                      Log::Critical,
 *                      MSRVERR_ACCEPT_FAILED,
 *                      "Accept failed with error %d; %s.",
 *                      errnum, strerror (errnum));
 * \endcode
 *
 * \return 0 if successful, otherwise a negative error code.
 ******************************************************************************/
int Log::message (
	const char* modulename,  /**< An identifier of the module writing to log. */
	int         fatality,    /**< Severity of the event.                      */
	int         errnum,       /**< Possible message number.                    */
	const char* msg,        /**< Message (as a format string for printf).    */
	...)
{
	/* Open ellipsis list. */
	va_list v_args;
	va_start (v_args, msg);

	int result = message (modulename, fatality, errnum, msg, v_args);
	
	/* Close the ellipsis handling. */
	va_end (v_args);

	return result;
}

/*******************************************************************************
 * \fn int Log::message (const char* modulename, int fatality, int errnum, const char* message, ...)=0;
 *
 * Writes a message to the log.
 ******************************************************************************/

int Log::info (const char* msg, ...)
{
	/* Open ellipsis list. */
	va_list v_args;
	va_start (v_args, msg);

	int result = message (mModuleDepth>0? mModules[mModuleDepth-1] : NULL,
						  Log::Info, 0, msg, v_args);

	/* Close the ellipsis handling. */
	va_end (v_args);

	return result;
}

/*******************************************************************************
 * Pushes module name to stack
 ******************************************************************************/
void Log::pushModule (const char* modulename)
{
	if (mModules.size() < mModuleDepth+1)
		mModules.resize (mModules.size()+1);
	mModules[mModuleDepth++] = modulename;
}

/*******************************************************************************
 * Pops module name from stack
 *
 * You must call this function before exiting the function where you
 * pushed the module.
 ******************************************************************************/
void Log::popModule ()
{
	if (mModuleDepth > 0)
		mModuleDepth--;
}


/*******************************************************************************
 * \fn int Log::write (const char* data, int len)=0;
 *
 * Writes to the log.
 ******************************************************************************/

/*******************************************************************************
 * Opens log to the given already open stream.
 *
 * \par Example:
 * \code
 *   LogFile log (stdout);
 *  log.message ("TEST", Log::Info, 0, "Log opened.");
 * \endcode
 ******************************************************************************/
LogFile::LogFile (
	FILE* stream /**< Stream to write the log to. */)
{
	mpLogStream = stream;
}

/*******************************************************************************
 * Opens a log file with the given name.
 *
 * The log will be appended to existing log file, if it
 * exists. Otherwise, the log file will be created.
 ******************************************************************************/
LogFile::LogFile (
	const char* filename /**< File to write the log to. */)
{
	mpLogStream = NULL;

	if (!filename)
		mpFilename = NULL; /* A problematic situation. */
	else
		mpFilename = strdup (filename);

	int result = open ();
	if (result < 0) {
		char buffer[1024];
		sprintf (buffer, "Opening log file failed with error %d.", -result);
		throw std::runtime_error (buffer);
	}
}

/*******************************************************************************
 * Closes the log file
 ******************************************************************************/
LogFile::~LogFile ()
{
	// mThreadLock.lock ();

	close ();

	delete mpFilename;

	// mThreadLock.unlock ();
}

/*******************************************************************************
 * Sets the file name
 *
 * Does not reopen the file with the new file name. You must \ref close() and
 * \ref open() the file.
 *
 * \return 0 if successful, otherwise a negative error code.
 ******************************************************************************/
int LogFile::setFilename (const char* filename)
{
	if (!filename)
		return MERR_FILE_NO_FILENAME;

	// mThreadLock.lock ();
	if (mpFilename)
		delete mpFilename;

	mpFilename = strdup (filename);

	// mThreadLock.unlock ();
	return 0;
}

/*******************************************************************************
 * Opens the log file
 *
 * The log will be appended to existing log file, if it
 * exists. Otherwise, the log file will be created.
 *
 * \return 0 if successful, otherwise a negative error code.
 ******************************************************************************/
int LogFile::open ()
{
	int result = 0;

	// mThreadLock.lock ();

	if (!mpFilename)
		result = MERR_FILE_NO_FILENAME;
	
	if (!result && mpLogStream)
		result = MERR_FILE_ALREADY_OPEN;

	if (!result) {
		if (! strcmp (mpFilename, "-")) {
			/* Open log to standard output. */
			mpLogStream = stdout;
			
		} else {
			/* Open the file. */
			FILE* logStream = fopen (mpFilename, "a");
			if (logStream == NULL)
				result = MERR_FILE_OPEN_FAILED;
			else
				mpLogStream = logStream;
		}
	}

	// mThreadLock.unlock ();
	return result;
}

/*******************************************************************************
 * Closes the log file
 ******************************************************************************/
void LogFile::close ()
{
	// mThreadLock.lock ();

	/* Close the file, if necessary. */
	if (mpLogStream && mpLogStream != stdout) {
		fclose (mpLogStream);
		mpLogStream = 0;
	}

	// mThreadLock.unlock ();
}

/*******************************************************************************
 * Writes a block of data to log stream
 *
 * \return 0 if successful, otherwise a negative error code.
 ******************************************************************************/
int LogFile::write (const char* data, int len)
{
	// mThreadLock.lock ();

	if (! mpLogStream)
		return MERR_FILE_NOT_OPEN;

	if (!data)
		return MERR_NULL_ARGUMENT;

	int written = fwrite (data, 1, len, mpLogStream);
	if (written < len)
		return MERR_FILE_SHORT_WRITE;

	// mThreadLock.unlock ();
	return 0;
}

/*******************************************************************************
 * Writes a log message
 *
 * \par Example:
 * \code
 * 		mpLog->message ("SERVER",
 *                      Log::Critical,
 *                      MSRVERR_ACCEPT_FAILED,
 *                      "Accept failed with error %d; %s.",
 *                      errnum, strerror (errnum));
 * \endcode
 *
 * \return 0 if successful, otherwise a negative error code.
 ******************************************************************************/
int LogFile::message (
	const char* modulename,  /**< An identifier of the module writing to log. */
	int         fatality,    /**< Severity of the event.                      */
	int         errnum,       /**< Possible message number.                    */
	const char* message,     /**< Message (as a format string for printf).    */
	va_list     v_args)
{
	static const char* fatalities[8] = {"EMERGENCY",
										"ALERT",
										"CRITICAL",
										"ERROR",
										"WARNING",
										"NOTICE",
										"INFO",
										"DEBUG"};

	// mThreadLock.lock ();

	if (! mpLogStream)
		return MERR_FILE_NOT_OPEN;

	if (!message)
		return MERR_NULL_ARGUMENT;

	if (fatality < 0 || fatality > Debug)
		return MERR_LOG_INVALID_FATALITY;

	/* Ensure that the error code is positive. */
	if (errnum < 0)
		errnum = -errnum;

	/* Write current time. */
	time_t currentTime = time (NULL);
	struct tm* pTimeTm = localtime (&currentTime);
	String logtime = strformat ("%04d/%02d/%02d %02d:%02d:%02d ",
								1900+pTimeTm->tm_year,
								pTimeTm->tm_mon+1,
								pTimeTm->tm_mday,
								pTimeTm->tm_hour,
								pTimeTm->tm_min,
								pTimeTm->tm_sec);

	/* Write log line header. */
	String logheader = strformat ("%s%s%s %d: ",
								  modulename? modulename:"",
								  modulename? " ":"",
								  fatalities[fatality],
								  errnum);

	/* Write the message with optional ellipsis. */
	String logmessage = vstrformat (message, v_args);

	/* Write the ending newline. */
	int written = fprintf (mpLogStream, (CONSTR) (logtime + logheader + logmessage + "\n"));

	if (written <= 0)
		return MERR_FILE_WRITE_FAILED;

	fflush (mpLogStream);

	fprintf (stderr, (CONSTR) (logmessage + "\n"));
	
	//mThreadLock.unlock ();
	return 0;
}


END_NAMESPACE;
