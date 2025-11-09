#ifndef __ATTRIBUTES_H__
#define __ATTRIBUTES_H__

#include <magic/mobject.h>
#include <magic/mstring.h>
#include <magic/mexception.h>

BEGIN_NAMESPACE (MagiC);

// Externals
template<class K, class V> class Map;


/** Inheritable support for key-seekable dynamic attributes.
 **/
class Attributed {
  public:
					Attributed		();
					~Attributed		();
	
	void			setAttribute	(const String& key, Object* value);

	/** Returns attribute by given key. Const version.
	 **/
	const Object&	getAttribute	(const String& key) const;

	/** Returns attribute by given key. Non-const version.
	 **/
	Object&			getAttribute	(const String& key);

	/** Returns attribute by given key. Const version.
	 **/
	const Object*	getAttributep	(const String& key) const;

	bool			hasAttribute	(const String& key) const;

  private:
		Attributed	(const Attributed& orig) {FORBIDDEN}
	void operator=	(const Attributed& orig) {FORBIDDEN}

	Map<String,Object>*	mAttributes;
};

END_NAMESPACE;

#endif
