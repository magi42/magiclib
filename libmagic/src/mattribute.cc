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

#include <magic/mattribute.h>
#include <magic/mmap.h>

BEGIN_NAMESPACE (MagiC);

Attributed::Attributed () : mAttributes (NULL)
{
}

Attributed::~Attributed ()
{
	delete mAttributes;
}

/** Sets an attribute value, replaces possible old value.
 *
 *  Note: Takes ownership of the object.
 **/
void Attributed::setAttribute (const String& key, Object* value)
{
	if (!mAttributes)
		mAttributes = new Map<String,Object> ();
	mAttributes->set (key, value);
}

const Object& Attributed::getAttribute (const String& key) const
{
	return mAttributes? (*mAttributes)[key] : *(Object*)NULL;
}

Object& Attributed::getAttribute	(const String& key)
{
	return mAttributes? (*mAttributes)[key] : *(Object*)NULL;
}

const Object* Attributed::getAttributep (const String& key) const
{
	return mAttributes? mAttributes->getp(key) : NULL;
}

bool Attributed::hasAttribute (const String& key) const
{
	return mAttributes? mAttributes->hasKey(key) : false;
}

END_NAMESPACE;
