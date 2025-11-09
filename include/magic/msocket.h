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

#ifndef __MAGIC_SOCKET_H__
#define __MAGIC_SOCKET_H__

#include <magic/miodevice.h>


/** A communication socket device.
 **/
class Socket : public IODevice {
  public:
					Socket				(int mode=0);
	virtual			~Socket				();

	int				socket				() const {return mSocket;}
	void			setSocket			(int socket);

	String			peerName			() const;
	uint			port				() const;
	uint			peerPort			() const;
	uint			address				() const;
	uint			peerAddress			() const;
	
	virtual bool	open				(int mode);
	virtual uint	size				() const					{return 0;}
	virtual int		at					() const					{return 0;}
	bool			reset				();
	virtual int		readBlock			(char* data, uint maxlen);
	virtual int		writeBlock			(const char* data, uint len);
	virtual int		readLine			(char* data, uint maxlen);
	int				writeBlock			(const String&);
	virtual char	getch				();
	virtual void	putch				(char ch);
	virtual void	ungetch				(char ch);

  protected:
	int     mSocket;
};


class ClientSocket : public Socket {
  public:
	virtual void	connect				(const String& host, uint port);
};
