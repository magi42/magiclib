/***************************************************************************
 *   This file is part of the MagiC++ library.                             *
 *                                                                         *
 *   Copyright (C) 1998-2004 Marko Grönroos <magi@iki.fi>                  *
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

#ifndef __MAGIC_MCONFIG_H__
#define __MAGIC_MCONFIG_H__

#include <magic/mpararr.h>

BEGIN_NAMESPACE (MagiC);

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                        ___                 o                             //
//                       /   \        _    __                               //
//                       |      __  |/ \  /   |  ___                        //
//                       |     /  \ |   | +-- | (   \                       //
//                       \___/ \__/ |   | |   |  ---/                       //
//                                        |      __/                        //
//////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * Configuration object
 ******************************************************************************/
class Config : public Object {
  public:

	struct KeyValuePair {
		String	key;
		String	value;
	};

	class Section : public Object {
	  public:
		Section	(const String& name);
		Section	();
		virtual	~Section	();
		
	  private:
		Array<StringKeyValue> mFields;
	};

  public:

					Config (const String& filename);
					Config ();
	virtual			~Config ();
	virtual int		load	(const String& filename);
	virtual int		save	(const String& filename="");

	int				newSection	(const String& name);
	int				set			(const String& section, const String& key, const String& value);

	const Section&	operator[]	(uint pos) const;
	Section&		operator[]	(uint pos);
	const Section&	operator[]	(const String& key) const;
	Section&		operator[]	(const String& key);
	
  private:
	String			mFilename;
	Array<Section>	mSections;
	
  private:
	Config (const Config& other) {}
	operator= (const Config& other) {}
};

END_NAMESPACE;

#endif
