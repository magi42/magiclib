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

#include "magic/mhtml.h"
#include "magic/mclass.h"

BEGIN_NAMESPACE (MagiC);

decl_module (html);

impl_dynamic (HTMLQuery, {Object});
impl_dynamic (HTMLPageBase, {Object});

///////////////////////////////////////////////////////////////////////////////

// unquote

void unquote (Map<String, String>& map) {
	for (MapIter<String,String> i (map); !i.exhausted(); i.next())
		i.value ().unquote ('%', String::QUOTE_HTML);
}

///////////////////////////////////////////////////////////////////////////////
//									HTMLQuery

HTMLQuery::HTMLQuery () {
	String contlen_tmp;
	contype		= getenv ("CONTENT_TYPE");
	contlen_tmp	= getenv ("CONTENT_LENGTH");
	docroot		= getenv ("DOCUMENT_ROOT");
	gwif		= getenv ("GATEWAY_INTERFACE");
	hosttype	= getenv ("HOSTTYPE");
	httpaccpt	= getenv ("HTTP_ACCEPT");
	httppragma	= getenv ("HTTP_PRAGMA");
	useragent	= getenv ("HTTP_USER_AGENT");
	pathinfo	= getenv ("PATH_INFO");
	pathtrans	= getenv ("PATH_TRANSLATED");
	querystr	= getenv ("QUERY_STRING");
	remaddr		= getenv ("REMOTE_ADDR");
	remhost		= getenv ("REMOTE_HOST");
	method		= getenv ("REQUEST_METHOD");
	sptname		= getenv ("SCRIPT_NAME");
	srvname		= getenv ("SERVER_NAME");
	srvport		= 80; // atoi (getenv ("SERVER_PORT"));
	srvprot		= getenv ("SERVER_PROTOCOL");
	srvsoft		= getenv ("SERVER_SOFTWARE");

	contlen = contlen_tmp.isNull()? 0 : atoi (contlen_tmp);
//	printf ("contlen=%d\n", (char*) contlen);

	if (method=="GET")
		splitpairs (query, querystr, '=', '&');
	else { // POST
		if (contlen > 0) {
			// Luetaan stdio:ta pituuden verran
			char* buff = new char [contlen+1];
			fgets (buff, contlen+1, stdin);
			querystr = buff;
			delete buff;
			splitpairs (query, querystr, '=', '&');
		}
	}

	// Konvertoidaan query-parametrien quotet
	unquote (query);

//	query.print (cout);
}

void HTMLQuery::printhidden () const {
	forStringMap (query, i)
		printf ("<INPUT TYPE=HIDDEN NAME='%s' VALUE='%s'>\n",
				(CONSTR) i.key (), (CONSTR) i.value ());
}

void HTMLQuery::any () {
	if (pathinfo.length()>5 && pathinfo.mid(pathinfo.length()-5) == ".html") {
		call (pathinfo.mid (1, pathinfo.length()-6)+String("_html"));
	} else
		printf ("Content-type: text/html\n\n"
				"<H1>Invalid path '%s'</H1>\n"
				"Page couldn't be found. Sorry!\n",
				(CONSTR) pathinfo);
	
}

int HTMLQuery::call (const String& pagename) {
	HTMLPageBase* pageobj = (HTMLPageBase*) dyncreate (pagename);
	if (pageobj) {
		pageobj->printqry (*this);
		delete pageobj;
		return 0;
	} else {
		printf ("Content-type: text/html\n\n"
				"<H1>Invalid page called '%s'</H1>\n"
				"Page couldn't be found. Sorry!\n",
				(CONSTR) pathinfo);
		return 1;
	}
}

END_NAMESPACE;
