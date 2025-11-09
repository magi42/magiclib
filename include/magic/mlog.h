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

#ifndef __MAGIC_MLOG_H__
#define __MAGIC_MLOG_H__

#include <magic/mlog.h>
#include <magic/merrors.h>
#include <magic/mmagisupp.h>
#include <magic/mstring.h>
#include <magic/mpararr.h>
#include <stdio.h>

BEGIN_NAMESPACE (MagiC);

/*******************************************************************************
 * Abstract log
 ******************************************************************************/
class Log {
  public:
	/** Fatality of a log event. Compatible with syslog(). */
	enum fatality {Emergency = 0, /**< System is unusable.                 */
				   Alert     = 1, /**< Action must be taken immediately.   */
				   Critical  = 2, /**< Critical conditions.                */
				   Error     = 3, /**< Error conditions.                   */
				   Warning   = 4, /**< Warning conditions.                 */
				   Notice    = 5, /**< Normal but insignificant condition. */
				   Info      = 6, /**< Informational.                      */
				   Debug     = 7  /**< Debug-level messages.               */};

	Log ();
	virtual ~Log ();
	
	virtual int			open		() = 0;
	virtual void		close		() = 0;
	int					message		(const char* modulename, int fatality, int errnum, const char* message, ...);
	int					info		(const char* message, ...);
	virtual int			message		(const char* modulename, int fatality, int errnum, const char* message, va_list v_args) = 0;
	void				pushModule	(const char* modulename);
	void				popModule	();
	
  protected:
	virtual int			write		(const char* data, int len) = 0;
	
  protected:
	String        mCurrentModule;
	Array<String> mModules;
	int           mModuleDepth;
};

/*******************************************************************************
 * Text-based log associated with a file stream
 *
 * The log format is as follows:
 *
 * \code <modulename> <fatality> <errnum>: <message> \endcode
 *
 * For example:
 *
 * \code MYMODULE WARNING 1234: This is a simple warning. \endcode
 ******************************************************************************/
class LogFile : public Log {
  public:
						LogFile		(FILE* stream = stdout);
						LogFile		(const char* filename);
	virtual 			~LogFile	();

	int					setFilename	(const char* filename);

	virtual int			open		();
	virtual void		close		();

	virtual int			message		(const char* modulename, int fatality, int errnum, const char* message, va_list v_args);
	
  protected:
	virtual int			write		(const char* data, int len);

  private:
	const char* mpFilename;
	FILE*		mpLogStream;
	//ThreadLock  mThreadLock;
};

/*******************************************************************************
 * Dummy log
 ******************************************************************************/
class DummyLog : public Log {
  public:
	virtual int			open		() {return 0;}
	virtual void		close		() {;}
	virtual int			message		(const char* modulename, int fatality, int errnum, const char* message, va_list v_args) {return 0;}

  protected:
	virtual int			write		(const char* data, int len) {return 0;}
};

END_NAMESPACE;

#endif
