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

#ifndef __MAGIC_MMAP_H__
#define __MAGIC_MMAP_H__

#include <iostream>
#include "magic/mobject.h"
#include "magic/mmagisupp.h"
#include "magic/mpararr.h"

BEGIN_NAMESPACE (MagiC);


// Externals
class DataOStream;

class MapPair;
class HashBucket;
class HashFunc;
class GenHash;
class GenHashIter;


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                    |   |            ----        o                        //
//                    |\ /|  ___   --  |   )  ___                           //
//                    | V |  ___| |  ) |---   ___| | |/\                    //
//                    | | | (   | |--  |     (   | | |                      //
//                    |   |  \__| |    |      \__| | |                      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

/** A key-value pair in @ref GenHash and thus in @ref Map; (internal).
 **/
class MapPair : public Object {
  public:
	const Comparable*	key;
	Object*				value;
	bool				isref;
						MapPair		() {key=NULL; value=NULL; isref=0;}
						~MapPair	();
	int					match		(const Comparable& okey) const {return (*key) == okey;}
	DataOStream&		operator>>	(DataOStream& out) const;
	void				check		() const;

  private:
	decl_dynamic (MapPair);
};


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                  ----             |   |             |                    //
//                 |      ___    _   |   |  ___   ____ | _                  //
//                 | --- /   ) |/ \  |---|  ___| (     |/ |                 //
//                 |   \ |---  |   | |   | (   |  \__  |  |                 //
//                 |___/  \__  |   | |   |  \__| ____) |  |                 //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * A hash bucket in @ref GenHash and thus in @ref Map; (internal).
 ******************************************************************************/
class HashBucket : public Object {
  public:
	MapPair		pair;
	HashBucket*	next;

					HashBucket	();
					~HashBucket	();
	DataOStream&	operator>>	(DataOStream& out) const;
	void			check		() const;

  private:
	decl_dynamic (HashBucket);
};

/*******************************************************************************
 * An interface for hash functions in @ref Genhash (and thus in @ref Map).
 ******************************************************************************/
class HashFunc {
  public:
	virtual			~HashFunc	() {}
	virtual void	hashfunc	(int hashsize) const = 0;
};

EXCEPTIONCLASS (not_found);

/*******************************************************************************
 * Generic hash table storage, used by @ref Map.
 ******************************************************************************/
class GenHash : public Object {
  public:

					GenHash		(int hsize=64, int flags=0) {make (NULL, hsize, flags);}
					GenHash		(HashFunc* hfunc, int hsize=16, int flags=0) {
						make (hfunc, hsize, flags);
					}
	void			make		(HashFunc* hashfunc, int hsize, int flags);

	void			set			(const Comparable* key, Object* value);
	const Object*	get			(const Comparable& key) const;
	DataOStream&	operator>>	(DataOStream& out) const;
	TextOStream&	operator>>	(TextOStream& out) const;
	ostream&		operator>>	(ostream& out) const;
	void			empty		();
	void			remove		(const Comparable& key);
	void			operator+=	(const GenHash& other);

	void			check		() const;

  protected:
	Array<HashBucket>	hash;
	int					hashsize;
	HashFunc*			hashfunc;
	bool				isref;

  private:
	decl_dynamic (GenHash);
	friend class GenHashIter;
};

/*******************************************************************************
 * @ref GenHash iterator.
 ******************************************************************************/
class GenHashIter {
  public:

						GenHashIter	(const GenHash* hash);

	void				first		();
	void				next		();
	Comparable&			getkeyv		();
	Object&				getvaluev	();
	const Comparable&	getkey		() const;
	const Object&		getvalue	() const;
	int					exhausted	() const {return exh;}

  protected:
	const GenHash*	hash;
	int				bucket;
	HashBucket*		currbucket;
	int				exh;
};

EXCEPTIONCLASS (map_item_not_found);

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                             |   |                                        //
//                             |\ /|  ___   --                              //
//                             | V |  ___| |  )                             //
//                             | | | (   | |--                              //
//                             |   |  \__| |                                //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

enum mapdefs {MAP_NONE=0, MAP_REF=1};

/*******************************************************************************
 *  Associative map, based on @ref GenHash.
 *
 *  The key class has to inherit @ref Comparable. The value class can
 *  be any inherit of @ref Object.
 *
 *  The inserted objects are owned by the Map by default, and are thus
 *  destroyed along with it, or during various replacement methods. By
 *  giving the MAP_REF flag to the constructor causes the objects not
 *  to be owned by the Map.
 *
 *  Map<String,String> has a shorthand macro StringMap. Many
 *  Map-handling functions, especially for StringMaps, are not
 *  included in the template, but are global. Such methods include
 *  toString(), splitpairs(), and joinpairs(). See section "globals"
 *  for more information, or the header mmap.h.
 *
 *  Maps can be iterated using @ref MapIter. Note also the forMap and
 *  forStringMap macros in mmap.h
 ******************************************************************************/
template <class keyclass, class valueclass>
class Map : public Object {
  public:
	/** Default constructor uses the default hash size of @ref GenHash.
	 **/
						Map			() {
							hash = new GenHash ();
							isref = 0;
							mFailByThrow=false;
							mFailByThrowOnce=false;
							mFailByNullOnce=false;
						}
	/** Constructor.
	 *
	 *  @param hashsize Hash size, should usually be larger than 16,
	 *  around is typical. Prime numbers are often recommended (which
	 *  16 and 64 are not).
	 *
	 *  @param flags Mode parameters. Currently defined are: MAP_REF
	 *  (causes objects to be not owned by the Map)
	 **/
						Map			(int hashsize, int flags) {
							hash = new GenHash (hashsize, flags);
							isref = flags & MAP_REF;
							mFailByThrow=false;
							mFailByThrowOnce=false;
							mFailByNullOnce=false;
						}
						Map (const Map& orig) {
							hash = new GenHash ();
							isref = 0;
							mFailByThrow=false;
							mFailByThrowOnce=false;
							mFailByNullOnce=false;

							// And copy
							operator= (orig);
						}
						~Map			() {delete hash;}

	/** Sets the _key_ to _value_; copies the given value object using
	 *  the copy constructor.
	 **/
	void				set				(const keyclass& key, const valueclass& value) {
		keyclass* nkey = new keyclass (key);
		valueclass* nvalue = NULL;
		if (isref)
			nvalue = const_cast<valueclass*> (&value);
		else
			nvalue = new valueclass (value);
		hash->set (nkey, nvalue);
	}

	/** Sets the _key_ to _value_; takes ownership of the passed object.
	 **/
	void				set				(const keyclass& key, const valueclass* value) {
		hash->set (new keyclass (key), const_cast<valueclass*> (value));
	}

	/** Removes an item from the map.
	 **/
	void				remove			(const keyclass& key) {
		hash->remove (key);
	}

	/** Queries whether the given key is in the map. */
	bool				hasKey			(const keyclass& key) const {
		return (hash->get (key)) != NULL;
	}

	/** Returns a const reference to object associated to the key.
	 *
	 *  For information about error handling and exceptions, see @ref
	 *  failByThrowOnce().
	 **/
	const valueclass&	operator[]		(const keyclass& key) const {
		return get (key);
	}

	/** Returns a non-const reference to object associated to the key.
	 *
	 *  For information about error handling and exceptions, see @ref
	 *  failByThrowOnce().
	 **/
	valueclass&	operator[]				(const keyclass& key) {
		return const_cast<valueclass&> (get (key));
	}

	/** Returns a const reference to object associated to the key.
	 *
	 *  For information about error handling and exceptions, see @ref
	 *  failByThrowOnce().
	 *
	 *  @see getv
	 **/
	const valueclass&	get				(const keyclass& key) const {
		const valueclass* pResult = (valueclass*) hash->get (key);
		if ((mFailByThrow || mFailByThrowOnce) && !mFailByNullOnce) {
			if (pResult == NULL) {
				try {
					const keyclass* dummy = &dynamic_cast<const keyclass&> (key);
					dummy = NULL; // Have to do this to avoid a silly warning
				} catch (...) {
					throw map_item_not_found ("Map item not found");
				}
				throw map_item_not_found (format("Map item '%s' not found", (CONSTR) key));
			}
		}
		mFailByThrowOnce = false;
		mFailByNullOnce  = false;
		return *pResult;
	}

	/** Returns a non-const reference to object associated to the key.
	 *
	 *  For information about error handling and exceptions, see @ref
	 *  failByThrowOnce().
	 *
	 *  @see get
	 **/
	valueclass&			getv			(const keyclass& key) {
		return const_cast<valueclass&> (get (key));
	}

	/***************************************************************************
	 *  Returns a const reference to object associated to the key, OR
	 *  if the key is not found from the map, returns the 'def'
	 *  reference back to the caller.
	 *
	 *  For information about error handling and exceptions, see @ref
	 *  failByThrowOnce().
	 *
	 *  @see get
	 ***************************************************************************/
	const valueclass&	getOr			(const keyclass& key, const valueclass& def) const {
		valueclass* tmp = (valueclass*) hash->get (key);
		return tmp? *tmp : def;
	}

 	/** Returns a const pointer to object associated to the key, or
	 *  NULL if the object was not found.
	 *
	 *  @see getvp
	 **/
	const valueclass*	getp			(const keyclass& key) const {
		return (valueclass*) hash->get (key);
	}

 	/** Returns a non-const pointer to object associated to the key,
	 *  or NULL if the object was not found.
	 *
	 *  @see getp
	 **/
	valueclass*			getvp			(const keyclass& key) {
		return (valueclass*) hash->get (key);
	}

	// Miscellaneous operators

	/** Implementation for @ref Object. */
	virtual DataOStream&			operator>>	(DataOStream& out) const {
		hash->operator>> (out);
		return out;
	}

	/** Implementation for @ref Object. */
	virtual ostream&			operator>>		(ostream& out) const {
		hash->operator>> (out);
		return out;
	}

	/** Implementation for @ref Object. */
	virtual TextOStream&		operator>>		(TextOStream& out) const {
		hash->operator>> (out);
		return out;
	}

	/** Union operator; adds the other Map to self. */
	Map<keyclass,valueclass>&	operator+=	(const Map<keyclass,valueclass>& other) {
		hash->operator+= (*other.hash);
		return *this;
	}

	/** Copy operator. */
	Map<keyclass,valueclass>&	operator=	(const Map<keyclass,valueclass>& other) {
		hash->empty ();
		hash->operator+= (*other.hash);
		isref = other.isref;
		mFailByThrow=other.mFailByThrow;
		mFailByThrowOnce=other.mFailByThrowOnce;
		mFailByNullOnce=other.mFailByNullOnce;
		return *this;
	}

	/** Destroys all the items from the Map.
	 **/
	void				empty			() {
		hash->empty ();
	}

	/** Returns the @ref GenHash of the Map.
	 **/
	const GenHash*		gethash			() const {return hash;}

	/** Implementation for @ref Object. */
	void				check			() const {hash->check();}

	/** Should the access-methods of Map report missing elements by
	 *  throwing an exception (true) or by returning a null reference
	 *  (false, which is the default).
	 *
	 *  Null references to objects can be checked by the global
	 *  isnull() function.
	 **/
	void failByThrow (bool bythrow=true) {mFailByThrow=bythrow;}

	/** Same as @ref failByThrow(true), but applies to only one access
	 *  call.
	 *
	 *  Giving parameter 'false' to this method cancels a previously
	 *  given failByThrowOnce(true) currently in effect.
	 **/
	void				failByThrowOnce	(bool bythrow=true) const {mFailByThrowOnce=bythrow;}

	/** Same as @ref failByThrow(false), but applies to only one
	 *  access call.
	 *
	 *  Giving parameter 'false' to this method cancels a previously
	 *  given failByNullOnce(true) currently in effect.
	 **/
	void				failByNullOnce	(bool bynull=true) const {mFailByNullOnce=bynull;}

  private:
	GenHash*		hash;
	bool			isref;
	bool			mFailByThrow;
	mutable bool	mFailByThrowOnce, mFailByNullOnce;
};

#define CMap Map

/*******************************************************************************
 *  Tests if the key is in the map. If it is, fetches the value,
 *  otherwise uses the default.
 *
 *  Usage: getOrDefault(myMap, "a_key", 0.1)
 ******************************************************************************/
#define getOrDefault(map,key,def)\
	(map.getp(key)? *map.getp(key):def)



//////////////////////////////////////////////////////////////////////////////
//                    |   |            ---                                  //
//                    |\ /|  ___   --   |   |   ___                         //
//                    | V |  ___| |  )  |  -+- /   ) |/\                    //
//                    | | | (   | |--   |   |  |---  |                      //
//                    |   |  \__| |    _|_   \  \__  |                      //
//////////////////////////////////////////////////////////////////////////////

/** Map iterator. Usage:
 *
 *  for (MapIter<String,String> iter (myMap); !iter.exhausted(); iter.next())
 *
 *  printf("%s=%s\n", (CONSTR) iter.key(), (CONSTR) iter.value());
 *
 *  , which prints the key-value-pairs of the map to stdout.
 *
 *  See mmap.h for additional macros forMap and forStringMap for
 *  somewhat easier iteration. For example:
 *
 *  forStringMap (myMap, iter) { printf("%s", (CONSTR) iter.value()); }
 *
 *  , which prints the values in the map to stdout.
 **/
template <class keyclass, class valueclass>
class MapIter {
  public:
	MapIter	(const Map<keyclass,valueclass>& map) : iter (map.gethash()) {;}

	/** Points the iterator to the first item in the @ref Map. */
	void				first		() {iter.first();}

	/** Moves the iterator to next item in the @ref Map. */
	void				next		() {iter.next();}

	/** Returns a const reference to the key in the current position
	 *  of the iterator.
	 **/
	const keyclass&		key			() const {return (const keyclass&) iter.getkey();}

	/** Returns a non-const reference to the key in the current position
	 *  of the iterator.
	 **/
	keyclass&			key			() {return (keyclass&) iter.getkeyv();}

	/** Returns a const reference to the value in the current position
	 *  of the iterator.
	 **/
	const valueclass&	value		() const {return (const valueclass&) iter.getvalue();}

	/** Returns a non-const reference to the value in the current position
	 *  of the iterator.
	 **/
	valueclass&			value		() {return (valueclass&) iter.getvaluev();}

	/** Returns 1 (true) if all the items in the mmap.have been
	 *  iterated, 0 (false) otherwise.
	 **/
	int					exhausted	() const {return iter.exhausted();}

  protected:
	GenHashIter	iter;
};

/** Easier iteration for @ref Map. For example:
 *
 *  forMap (String, String, myMap, iter) { printf("%s", (CONSTR) iter.value()); }
 *
 *  , which prints the values in the @ref Map<String,String> to stdout.
 **/
#define forMap(keytype,valuetype,mapname,varname) \
	for (MapIter<keytype,valuetype> varname (mapname); !varname.exhausted(); varname.next())



//////////////////////////////////////////////////////////////////////////////
//                ----         o             |   |                          //
//               (      |          _         |\ /|  ___   --                //
//                ---  -+- |/\ | |/ \   ___  | V |  ___| |  )               //
//                   )  |  |   | |   | (   \ | | | (   | |--                //
//               ___/    \ |   | |   |  ---/ |   |  \__| |                  //
//                                      __/                                 //
//////////////////////////////////////////////////////////////////////////////

// StringMap is just an alias
typedef Map<String,String> StringMap;

// Reads String Map from file. Throws exception file_not_found if
// opening fails.
StringMap readStringMap (const String& filename);

// Reads String Map from stream
StringMap readStringMap (TextIStream& in, const char* path=NULL);

void writeStringMap (const Map<String,String>& map, TextOStream& out);

/** Creates a @ref String representation of the Map in format:
 *  {"xxx"="yyy", "zzzz"="aaaa"}
 **/
String toString (const StringMap& map);

/** Constructs the map from string containing key-value-pairs in format:
 *  "key1=value1&key2=value2&key3=value3".
 *
 *  Reverse for @ref joinPairs().
 **/
void splitpairs (StringMap& trg, const String& source, char psep='=', char rsep='&');

/** Constructs a string from the map, representing the key-value-pairs in
 *  format: "key1=value1&key2=value2&key3=value3".
 *
 *  Reverse for @ref splitPairs().
 **/
String joinpairs (const StringMap& src, char psep='=', char rsep='&');


/** Easier iteration for @ref Map<String,String> alias StringMap. For
 *  example:
 *
 *  forStringMap (myMap, iter) { printf("%s", (CONSTR) iter.value()); }
 *
 *  , which prints the values in the StringMap to stdout.
 **/
#define forStringMap(mapname,varname) \
	for (MapIter<String,String> varname (mapname); !varname.exhausted(); varname.next())

END_NAMESPACE;

#endif
