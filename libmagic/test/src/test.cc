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

#include <stdio.h>

#include <magic/mstring.h>
#include <magic/mapplic.h>

#include "tests.h"

////////////////////////////////////////////////////////////////////////////////

int printout=true;

/******************************************************************************/
void testf (CONSTR funcname,   /* Name of the function to be tested. */
			bool (* func) ())  /* Function to be tested.             */
{
	if (printout) {
		printf ("Test %-30s...", funcname);
		fflush (stdout);
	}

	// Call the test function
	bool ok = func ();
	
	if (printout)
		printf (ok? "OK!\n" : "\nFAIL!\n");
}

#define test(fname) testf (#fname, fname)

/******************************************************************************/
using namespace MagiC;

Main () {
	assertmode = ASSERT_CRASH;
	printf ("---------------------------------------------------\n");

	for (int i=0; i<1; i++) {

		// String tests
		test (string_basicTests);

		// IODevice tests
		test (iodevice_fileWriting);

		// Stream tests
		test (stream_fileStream);
		test (stream_stringStream);

		printout = false;
	}

	printf ("---------------------------------------------------\n");
	printf ("MagiC++ library test program exiting...\n");
}

