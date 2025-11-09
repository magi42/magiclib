/***************************************************************************
 *   This file is part of the MagiC++ library.                             *
 *                                                                         *
 *   Copyright (C) 1998-2002 Marko Grönroos <magi@iki.fi>                  *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "magic/mstring.h"
using namespace MagiC;

bool string_basicTests ()
{
	String str1;
	String str2 = "foobar";
	String str3 ("abc\000def", 7);

	if (str2.length() != 6)
		return false;
	
	if (str3.length() != 7)
		return false;

	// Catenation
	String str4 = str1+str2+str3;
	str4 += "foo";
	str4 += 'a';
	if (str4.length() != str1.length() + str2.length() + str3.length()+4)
		return false;

	if (str4 != "foobarabc\000deffooa")
		return false;

	// Formatting
	String formatted = strformat ("Test %d: %s\n", 42, "something");
	if (formatted != "Test 42: something\n")
		return false;

	return true;
}

bool string_arg ()
{
	cout << "'"
		 << String("\t  This   contains    \n some whitespace.  \n").simplifyWhiteSpace()
		 << "'\n";
	cout << "'" << String("   ").simplifyWhiteSpace() << "'\n";
	
	cout << String ("%2 is some %1 text").arg(42, 0, 16).arg("foo") << "\n";
	cout << String ("%2 is more %1 text %3").arg(42).arg("foo").arg(123.456, 20, 'g', 10) << "\n";

	return true;
}
