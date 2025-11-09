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

#ifndef __MAGIC_MAPPLIC_H__
#define __MAGIC_MAPPLIC_H__

#include "magic/mobject.h"
#include "magic/mclass.h"
#include "magic/mmap.h"

BEGIN_NAMESPACE (MagiC);

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//               _             | o                 o                        //
//              / \   --   --  |    ___   ___   |           _               //
//             /   \ |  ) |  ) | | |   \  ___| -+- |  __  |/ \              //
//             |---| |--  |--  | | |     (   |  |  | /  \ |   |             //
//             |   | |    |    | |  \__/  \__|   \ | \__/ |   |             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

int main (int argc, char** argv);

/** An application-class that can be inherited instead of using main().
 *
 *  If you inherit this, remember to link the app library with switch
 *  -lapp. The application framework automatically calls the main()
 *  method of this class.
 *
 *  NOTE: The applicatino MUST use the decl_dynamic macro to enable house-RTTI.
 **/
class Application : public Object {
	decl_dynamic (Application);
  protected:
	Array<String>	mParams;
	StringMap		mParamMap;
	int				mArgc;
	char**			mArgv;
	String			mAppPath;
	String			mDataDir;
	Array<String>	mConfigPath;
	int				mTried;

  public:
						Application		() : mTried (0) {}

	/** Always implement this OR the other main () below. OBSOLETE.
	 *
	 *  @param params Parameters passed from command line. This method
	 *  is obsolete because these parameters are now a member.
	 **/
	virtual void		main			(const Array<String>& params) {mTried++;}

	/** Always implement this OR the other main (const Array<String>&) above.
	 **/
	virtual void		main			() {mTried++;}

	void				readConfig		(const String& filename);

	/** Returns the parameter map of the application.
	 **/
	const StringMap&	paramMap		() const {return mParamMap;}

	/** Returns the command-line parameter list.
	 **/
	const Array<String>& params			() const {return mParams;}

	const String&		appPath			() const {return mAppPath;}
	const String&		dataDir			() const {return mDataDir;}

	void				setParams		(int argc, char** argv);

	int&				argc			() {return mArgc;}
	char**				argv			() const {return mArgv;}

  private:
	void				initConfigPath	();
	int					getTried		() const {return mTried;}
};

/**
 *  @short Main() {...} macro to replace main()
 *
 *  @see Application
 **/
#define Main()\
class MainClass : public Application {\
	decl_dynamic (MainClass);\
  public:\
	void main ();\
};\
impl_dynamic (MainClass, "{Application}");\
void MainClass::main ()

extern Class* appclass;

END_NAMESPACE;

#endif
