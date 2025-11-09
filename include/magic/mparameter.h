/***************************************************************************
 *   This file is part of the MagiC++ library.                             *
 *                                                                         *
 *   Copyright (C) 1997-2002 Marko Grönroos <magi@iki.fi>                  *
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

#ifndef __PARAMETER_H__
#define __PARAMETER_H__

#include<magic/mstring.h>
#include<magic/mmap.h>

BEGIN_NAMESPACE (MagiC);

///////////////////////////////////////////////////////////////////////////////
//     ___               ----                                                //
//     |  \          _   |   )  ___       ___         ___   |   ___          //
//     |   | \   | |/ \  |---   ___| |/\  ___| |/|/| /   ) -+- /   ) |/\     //
//     |   |  \  | |   | |     (   | |   (   | | | | |---   |  |---  |       //
//     |__/    \_/ |   | |      \__| |    \__| | | |  \__    \  \__  |       //
//            \_/                                                            //
///////////////////////////////////////////////////////////////////////////////

/** Dynamic field (or object) type parameter.
 *
 **/
class DynParameter {
  public:
	/** Default constructor. FORBIDDEN - included only for technical reasons. */
	DynParameter () {FORBIDDEN}

	/** The constructor to use. */
					DynParameter (const String& name,		/**< Name of the parameter. */
								  const String& shortd,		/**< Short description. */
								  int maxlen,				/**< Maximum length (in characters). */
								  const String& clsname		/**< Class name of the inheritor. */
						) : mName (name), mShortDescription (shortd), mParamClassName (clsname), mMaxlen(maxlen) {}

	virtual ~DynParameter () {}

	/** Returns the name of the parameter. */
	const String&	name		() const {return mName;}

	/** Returns a short description of the parameter. */
	const String&	shortDesc	() const {return mShortDescription;}

	/** Returns parameter's class name. */
	const String&	paramClass	() const {return mParamClassName;}

	/** Returns maximum length (in characters) of the value's string representation. */
	int				maxlen		() const {return mMaxlen;}

	/** Checks if the given string representation is a valid
	 *  value. Returns true if it is, false if not.
	 **/
	virtual bool	checkValue	(const String& value) const {return int(value.length()) <= mMaxlen;}
	
  private:
	String	mName;				/**< Name of the parameter. */
	String	mShortDescription;	/**< Short description of the parameter. */
	String	mParamClassName;	/**< Classname of the parameter. */
	int		mMaxlen;			/**< Maximum length of the parameter's value, in characters. */
};


/////////////////////////////////////////////////////////////////////////////////
//  ----         o             ----                                            //
// (      |          _         |   )  ___       ___         ___   |   ___      //
//  ---  -+- |/\ | |/ \   ___  |---   ___| |/\  ___| |/|/| /   ) -+- /   ) |/\ //
//     )  |  |   | |   | (   \ |     (   | |   (   | | | | |---   |  |---  |   //
// ___/    \ |   | |   |  ---/ |      \__| |    \__| | | |  \__    \  \__  |   //
//                        __/                                                  //
/////////////////////////////////////////////////////////////////////////////////

class StringParameter : public DynParameter {
  public:
	virtual bool	checkValue	(const String& value) const {return DynParameter::checkValue(value);}
	
  protected:
	StringParameter (const String& name, const String& shortd, int maxlen) : DynParameter (name, shortd, maxlen, "string") {}
};


////////////////////////////////////////////////////////////////////////////////////
// ___                    |       ----                                            //
// |  \             |     |  ___  |   )  ___       ___         ___   |   ___      //
// |   |  __  |   | |---  | /   ) |---   ___| |/\  ___| |/|/| /   ) -+- /   ) |/\ //
// |   | /  \ |   | |   ) | |---  |     (   | |   (   | | | | |---   |  |---  |   //
// |__/  \__/  \__! |__/  |  \__  |      \__| |    \__| | | |  \__    \  \__  |   //
////////////////////////////////////////////////////////////////////////////////////

class DoubleParameter : public DynParameter {
  public:
					DoubleParameter (const String& name, const String& shortd, int maxlen, double minv, double maxv, double defv=-345535.234) : DynParameter (name, shortd, maxlen, "float"), mMin(minv), mMax(maxv), mDefault(defv) {}
	virtual bool	checkValue		(const String& value) const {return DynParameter::checkValue(value);}
	virtual String	defaultString	() const {return (mDefault==-345535.234)? String(""):String(mDefault);}
	
  private:
	double	mMin;
	double	mMax;
	double	mDefault;
};

class IntParameter : public DynParameter {
  public:
					IntParameter	(const String& name, const String& shortd, long minv, long maxv, long defv=-3475235) : DynParameter (name, shortd, 15, "int"), mMin(minv), mMax(maxv), mDefault(defv) {}
	virtual bool	checkValue		(const String& value) const {return DynParameter::checkValue(value);}
	virtual String	defaultString	() const {return (mDefault==-3475235)? String(""):String(mDefault);}
	
  private:
	int	mMin;
	int	mMax;
	int	mDefault;
};

class BoolParameter : public DynParameter {
  public:
					BoolParameter	(const String& name, const String& shortd, int defv=-1) : DynParameter (name, shortd, 1, "int"), mDefault(defv) {}
	virtual bool	checkValue		(const String& value) const {return DynParameter::checkValue(value);}
	virtual String	defaultString	() const {return (mDefault==-1)? String(""):String(mDefault);}
	
  private:
	int		mDefault;
};


///////////////////////////////////////////////////////////////////////////////
//   --- ----                                            o               |   //
//    |  |   )  ___       ___         ___   |   ___        ___  ___      |   //
//    |  |---   ___| |/\  ___| |/|/| /   ) -+- /   ) |/\ |   / /   )  ---|   //
//    |  |     (   | |   (   | | | | |---   |  |---  |   |  /  |---  (   |   //
//   _|_ |      \__| |    \__| | | |  \__    \  \__  |   | /__  \__   ---|   //
///////////////////////////////////////////////////////////////////////////////

/** Interface for dynamically parameterizable objects.
 *
 **/
class IParameterized {
  public:
	virtual ~IParameterized () {}
	
	/** Returns description of parameters required by the training
	 *  algorithm.
	 **/
	virtual Array<DynParameter>*	parameters	() const {MUST_OVERLOAD; return NULL;}

	/** Initializes the object with given dynamic parameters.
	 **/
	virtual void					init		(const StringMap& params) {MUST_OVERLOAD}
};

/** The INITPARAMS macro handles and prints errors in IParameterized::init() function.
 *
 */
#define INITPARAMS(mapname, initcode) \
	try { const_cast<StringMap&>(mapname).failByThrow(); initcode \
	} catch (Exception e) {\
		String errorstr = e.what();\
		errorstr += "\nInitialization parameter missing. Parameters are:\n";\
		Array<DynParameter>* pars = parameters ();\
		for (int i=0; i<pars->size(); i++)\
			errorstr += (*pars)[i].name () + " ";\
		errorstr += '\n';\
		throw Exception (errorstr);\
	}

END_NAMESPACE;

#endif
