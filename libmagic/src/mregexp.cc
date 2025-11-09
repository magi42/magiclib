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

#include <magic/mobject.h>
#include <magic/mstring.h>
#include <magic/mmagisupp.h>
#include <magic/mpararr.h>
#include <magic/mregexp.h>

// impl_dynamic (RegExp, {Object});

	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////// RegExp:Object ////////////////////////////////

BEGIN_NAMESPACE (MagiC);

RegExp::~RegExp () {
	if (regt) {
		regfree (regt);
		delete regt;
		regt=NULL;
	}
}

int RegExp::make (const char* expr) {
	this->~RegExp ();
	regt = new regex_t;

	if ((errcode = regcomp (regt, expr, REG_EXTENDED)))
		throw invalid_format (format ("%%%%%%RegExp Error:%s\n", (CONSTR) geterror ()));

	return errcode;
}
	
int RegExp::match (const char* str) {
	return !(errcode = regexec (regt, str, 0, NULL, REG_NOSUB));
}

int RegExp::match (const String& str, Array<String>& results) {
	regmatch_t matches [20];
	errcode = regexec (regt, str, 20, matches, 0);
	if (errcode) {
		if (errcode == REG_NOMATCH)
			return 0;
		else
			throw invalid_format (geterror());
	}

	int rescnt=0;
	for (; rescnt<19; rescnt++)
		if (matches[rescnt].rm_so > int(str.length()) || matches[rescnt].rm_eo > int(str.length()) ||
			matches[rescnt].rm_so < 0 || matches[rescnt].rm_eo < 0 ||
			matches[rescnt].rm_so > matches[rescnt].rm_eo)
			break;

	results.resize (rescnt);

	for (int i=0; i<rescnt; i++)
		results[i] = str.mid (matches[i].rm_so, matches[i].rm_eo-matches[i].rm_so);

	return 1;
}

String RegExp::geterror () const {
	char errbuf [256];
	regerror (errcode, regt, errbuf, 256);
	return errbuf;
}

END_NAMESPACE;
