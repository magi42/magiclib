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

#include "magic/mobject.h"
#include <stdio.h>
#include <string.h>
#include <fstream>		// Needed by readStringMap, etc.
#include "magic/mmap.h"
#include "magic/mstream.h"
#include "magic/mtextstream.h"
#include "magic/mdatastream.h"
#include "magic/mclass.h"

BEGIN_NAMESPACE (MagiC);

decl_module (map);

// impl_dynamic (GenHash, {Object});



//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                    |   |            ----        o                        //
//                    |\ /|  ___   --  |   )  ___    | _                    //
//                    | V |  ___| |  ) |---   ___| | |/                     //
//                    | | | |   | |--  |     |   | | |                      //
//                    |   |  \__| |    |      \__| | |                      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

impl_dynamic (MapPair, {Object});

MapPair::~MapPair	() {
	delete key;
	key = NULL;
	if (!isref)
		delete value;
	value = NULL;
}

DataOStream& MapPair::operator>> (DataOStream& out) const {
	if (key)
		out.name("key") << *key;
	if (value)
		out.name("value") << *value;
	return out;
}

void MapPair::check () const {
	ASSERT (value!=(void*)0x1);
}



//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//          |   |             |    ----              |                      //
//          |   |  ___   ____ | _  |   )        ___  |    ___   |           //
//          |---|  ___| (     |/ | |---  |   | |   \ | / /   ) -+-          //
//          |   | |   |  \__  |  | |   ) |   | |     |/  |---   |           //
//          |   |  \__| ____) |  | |___   \__! \___/ | \  \__    \          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

impl_dynamic (HashBucket, {Object});

HashBucket::HashBucket	() {
	next=NULL;
}

HashBucket::~HashBucket	() {
	delete next;
	next = NULL;
}

DataOStream& HashBucket::operator>> (DataOStream& out) const {
	out << pair;
	return out;
}

void HashBucket::check () const {
	pair.check ();
	if (next)
		next->check ();
}



//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                  ----             |   |             |                    //
//                 |      ___    __  |   |  ___   ____ | _                  //
//                 | --- /   ) |/  | |---|  ___| (     |/ |                 //
//                 |   \ |---  |   | |   | |   |  \__  |  |                 //
//                 |___/  \__  |   | |   |  \__| ____) |  |                 //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

impl_dynamic (GenHash, {Object});

void GenHash::make (HashFunc* hfunc, int hsize, int flags) {
	hashsize = hsize;
	hash.resize (hashsize);
	hashfunc = hfunc;
	isref = flags;
}

void GenHash::set (const Comparable* key, Object* value) {
	// Calculate the hash value of the key. This is a feature of
	// Comparable-inherited objects.
	int hval = key->hashfunc (hashsize);

	// Find the bucket according to the hash value
	HashBucket* bucket = hash.getp (hval);

	// If there is a bucket in that hash slot, add into it
	if (bucket)
		// As long as there is something in the bucket
		for (;bucket; bucket=bucket->next) {
			// Check if this is the matching bucket
			if (bucket->pair.match (*key)) {
				// It existed. Wow.
				delete bucket->pair.value;	// Replace the old value
				delete key;					// Dispose the excess key
				break;
			}

			// That wasn't the right bucket. If there are no more
			// buckets, create a new one and exit the search
			if (!bucket->next) {
				// Add the item to the new bucket
				bucket->next = new HashBucket;	// Create new bucket
				bucket = bucket->next;			// Move on to the new bucket
				bucket->pair.key = key;			// Set the key
				bucket->pair.isref = isref;		// Clone the global isref setting
				break;
			}
		}
	else {
		// Add a new bucket in into the hash array
		hash.put (bucket = new HashBucket, hval);
		bucket->pair.key = key;
	}

	// Finally set the value
	bucket->pair.value = value;
}

const Object* GenHash::get (const Comparable& key) const {
	for (const HashBucket* sanko = hash.getp (key.hashfunc(hashsize)); sanko; sanko=sanko->next) {
		ASSERT (sanko->pair.key != (void*)0x1);
		ASSERT (sanko->pair.value != (void*)0x1);
		if (sanko->pair.match (key))
			return sanko->pair.value;
	}

	return NULL;

	/*
	if (0) {
		// String-avaimet kerrotaan...
		if (key.is_a ("String")) {
			const String& skey = static_cast<const String&> (key);
			throw not_found (format ("CMap object not found with key '%s'", (CONSTR) skey));
		} else
			throw not_found ("CMap object not found with a key");
	} else {
		// Ilmoitetaan virheest� palauttamalla NULL
		return NULL;
	}
	*/
}

void GenHash::empty () {
	hash.empty ();
}

void GenHash::remove (const Comparable& key) {
	HashBucket* prev = NULL;
	int bucket = key.hashfunc (hashsize);
	for (HashBucket* sanko = hash.getp (bucket); sanko; sanko=sanko->next) {
		if (sanko->pair.match (key)) {
			if (prev) {
				// Ei olla ensimm�inen
				prev->next = sanko->next;
				sanko->next = NULL;
				delete sanko;
			} else {
				// Ollaan ensimm�inen

				if (sanko->next) {
					// Per�ss� on muita
					HashBucket* next = sanko->next;
					sanko->next = NULL;
					hash.put (next, bucket);
					delete sanko;
				} else {
					// Ollaan ainoa
					hash.remove (bucket);
					// hash.empty ();
				}
			}
			break;
		}
		prev = sanko;
	}
}

void GenHash::operator+= (const GenHash& other) {
	for (GenHashIter i (&other); !i.exhausted(); i.next())
		set (static_cast<Comparable*> (i.getkey().clone ()), i.getvalue().clone ());
}

DataOStream& GenHash::operator>> (DataOStream& out) const {
	out.name ("hashsize") << hashsize;
	out.name ("isref") << (int)isref;
	out.name ("hash") << hash;
	return out;
}

/*******************************************************************************
 * Prints the hash to output stream.
 ******************************************************************************/
ostream& GenHash::operator>> (ostream& out) const
{
	for (GenHashIter iter (this); !iter.exhausted(); iter.next())
		out << iter.getkey() << "=" << iter.getvalue() << "\n";
	return out;
}

/*******************************************************************************
 * Prints the hash to output stream.
 ******************************************************************************/
TextOStream& GenHash::operator>> (TextOStream& out) const
{
	for (GenHashIter iter (this); !iter.exhausted(); iter.next())
		out << iter.getkey() << "=" << iter.getvalue() << "\n";
	return out;
}

void GenHash::check () const {
	for (int i=0; i<hash.size(); i++)
		if (hash.getp(i)) // Check only non-null buckets
			hash[i].check (); // Check the bucket
}



/*******************************************************************************
 **                                                                           **
 **          ----            |   |             |    ---                       **
 **         |      ___    _  |   |  ___   ____ | _   |   |   ___  | _         **
 **         | --- /   ) |/ | |---|  ___| (     |/ |  |  -+- /   ) |/          **
 **         |   \ |---  |  | |   | |   |  \__  |  |  |   |  |---  |           **
 **         |___/  \__  |  | |   |  \__| ____) |  | _|_   \  \__  |           **
 **                                                                           **
 *******************************************************************************/

GenHashIter::GenHashIter (const GenHash* ghash) {
	hash = ghash;
	first ();
}

void GenHashIter::first () {
	bucket = 0;
	currbucket = NULL;
	exh = 0;
	next ();
}

void GenHashIter::next () {
	if (currbucket) {
		if ((currbucket = currbucket->next))
			return;
		else
			bucket++;
	}

	while (bucket < hash->hash.size() && (currbucket=(HashBucket*)(hash->hash.getp (bucket))) == NULL)
		bucket++;

	if (bucket >= hash->hash.size())
		exh = 1;
}

Comparable& GenHashIter::getkeyv () {
	if (currbucket)
		return const_cast<Comparable&> (*(currbucket->pair.key));
	else
		return *(Comparable*)NULL;
}

Object& GenHashIter::getvaluev () {
	if (currbucket)
		return *(currbucket->pair.value);
	else
		return *(Object*) NULL;
}

const Comparable& GenHashIter::getkey () const {
	if (currbucket)
		return *(currbucket->pair.key);
	else
		return *(const Comparable*) NULL;
}

const Object& GenHashIter::getvalue () const {
	if (currbucket)
		return *(currbucket->pair.value);
	else
		return *(const Object*) NULL;
}



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

void splitpairs (CMap<String,String>& trg, const String& source,
				 char psep, char rsep) {

	// Tyhjennet��n kakat pois
	trg.empty ();

	// Halkaistaan ensin kent�t erilleen ja laitetaan v�liaikaiseen vektoriin
	Array<String> tmp;
	source.split (tmp, rsep);

	// K�yd��n parit sis�lt�v�t merkkijonot l�pi
	for (int i=0; i<tmp.size(); i++) {
		// Halkaistaan pari kahtia
		int pos = tmp[i].find (psep);
		String left = tmp[i].left (pos);
		String right = tmp[i].mid (pos+1);
		trg.set (left, right);
	}
}

String joinpairs (const CMap<String,String>& src, char psep, char rsep) {
	Array<String> arr;
	forStringMap (src, i)
		arr.add (new String (format ("%s%c%s", (CONSTR)i.key(), psep, (CONSTR)i.value())));

	String result;
	result.join (arr, rsep);
	return result;
}

// Print operation to string
String toString (const StringMap& map) {
	String result = "{";
	bool first=true;
	forStringMap (map, i) {
		if (first)
			first = false;
		else
			result += ", ";
		result += i.key() + "=\042" + i.value() + "\042";
	}
	result += "}";

	return result;
}



//////////////////////////////////////////////////////////////////////////////
//                ----         o             |   |                          //
//               (      |          _         |\ /|  ___   --                //
//                ---  -+- |/\ | |/ \   ___  | V |  ___| |  )               //
//                   )  |  |   | |   | (   \ | | | (   | |--                //
//               ___/    \ |   | |   |  ---/ |   |  \__| |                  //
//                                      __/                                 //
//////////////////////////////////////////////////////////////////////////////

StringMap readStringMap (const String& filename) {
	TextIStream in (new File (filename));
	if (!in)
		throw file_not_found (strformat ("Could not open file '%s' for reading StringMap",
										 (CONSTR) filename));

	// Determine current path
	String path = "./";
	if (filename.findRev("/") != -1)
		path = filename.mid (0, filename.findRev("/")+1);

	// Call stream reader function
	return readStringMap (in, path);
}

StringMap readStringMap (TextIStream& in, const char* path) {
	StringMap result;
	String paramStr;
	String section;
	String buffer;
	Array<String> regpar;
	Array<String> pair;		// Key-value pair

	while (in.readLine (buffer)) {
		buffer.chop(); // Cleanup trailing whitespaces

		// Map definition can be terminated with this special
		// row. Case sensitive.
		if (buffer == "# end-of-map")
			break;

		if (buffer.regmatch ("^INCLUDE\\:? *(.+)", regpar)) {
			// Include file directive. Determine path.
			String curPath="./";
			if (path)
				curPath = path;

			// Read the include file recursively
			result += readStringMap (curPath+regpar[1]);

		} else if (buffer.regmatch ("^\\[([a-zA-Z0-9]*)\\]", regpar)) {
			// Section name line
			section = (regpar[1]=="]")? String():regpar[1];

		} else if (buffer.regmatch ("^[ \t]*\\#")) {
			// Comment line, bypass
			continue;
		} else {
			// Check if key=value pair
			buffer.split (pair, '=');
			if (pair.size() == 2) {
				// Cleanup key and value
				String key   = pair[0].stripWhiteSpace();
				String value = pair[1].stripWhiteSpace();

				// If neither is empty, add to map
				if (!isempty(key) && !isempty(value)) {
					// Add section prefix to key
					if (!isempty(section))
						key = section+"."+key;
					result.set (key, value); // Add to map
				}
			} else {
				// Unknown line
			}
		}
	} // while not EOF

	return result;
}

void writeStringMap (const StringMap& map, TextOStream& out)
{
	// TODO: Handle sectioning correctly

	// Write everything under the default section
	out << "[]\n";

	forStringMap (map, mapi)
		out << mapi.key() << "=" << mapi.value() << "\n";
}


END_NAMESPACE;
