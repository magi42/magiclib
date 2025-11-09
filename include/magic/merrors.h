/***************************************************************************
 *   This file is part of the MagiCLib++ library.                          *
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

#ifndef __MAGIC_MERRORS_H__
#define __MAGIC_MERRORS_H__

/*******************************************************************************
 * General errors
 ******************************************************************************/

#define MERR_INVALID_ARGS           1
#define MERR_TIMEOUT                2
#define MERR_NULL_ARGUMENT          3
#define MERR_LOG_INVALID_FATALITY   4

/*******************************************************************************
 * File errors
 ******************************************************************************/
#define MERR_FILE_BASE              100
#define MERR_FILE_NO_FILENAME       (MERR_FILE_BASE)
#define MERR_FILE_ALREADY_OPEN      (MERR_FILE_BASE+1)
#define MERR_FILE_OPEN_FAILED       (MERR_FILE_BASE+2)
#define MERR_FILE_NOT_OPEN          (MERR_FILE_BASE+3)
#define MERR_FILE_SHORT_WRITE       (MERR_FILE_BASE+4)
#define MERR_FILE_WRITE_FAILED      (MERR_FILE_BASE+5)

/*******************************************************************************
 * Thread errors
 ******************************************************************************/
#define MERR_THREAD_BASE            1000
#define MERR_THREAD_CREATE_FAILED   (MERR_THREAD_BASE)
#define MERR_THREAD_JOIN_FAILED     (MERR_THREAD_BASE+1)
#endif
