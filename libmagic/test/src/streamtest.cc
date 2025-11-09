/*******************************************************************************
 *    This file is part of the MagiC++ library.                                *
 *                                                                             *
 *    Copyright (C) 1998-2002 Marko Grönroos <magi@iki.fi>                     *
 *                                                                             *
 *******************************************************************************
 *                                                                             *
 *    This program is free software; you can redistribute it and/or modify     *
 *    it under the terms of the GNU General Public License as published by     *
 *    the Free Software Foundation; either version 2 of the License, or        *
 *    (at your option) any later version.                                      *
 *                                                                             *
 ******************************************************************************/

#include <magic/mstring.h>
#include <magic/mtextstream.h>
#include <math.h>

using namespace MagiC;

/*******************************************************************************
* NAME:        stream_fileStream
*
* DESCRIPTION: Runs TextOStream tests with a File device.
*
* RETURNS:     true if successful, false on failure.
*******************************************************************************/
bool stream_fileStream ()
{
	/**************************************************************************/
	/*                        Writing to file                                 */
	
	/* Create a test file for writing. */
	File file ("/tmp/testfile.txt", IO_Writable | IO_Truncate);

	/* Attach the file device to a text stream. */
	MagiC::TextOStream out (file);

	/* Write stuff to the stream (and consequently to the file). */
	out << "foobar\n" << 10 << " " << 42.42 << "\n   \n" << "-1.2345E-10" << "\n"; 
 
	/* PROBLEM: The stream will not know that the file is closed. */
	file.close ();

	/**************************************************************************/
	/*                        Reading from file                               */

	/* Open the same file for reading. */
	file.open (IO_Readable);

	/* Attach the file device to a text input stream. */
	MagiC::TextIStream in (file);

	/* Read from stream word by word. */
	String inbuff;
	in >> inbuff; // Read "foobar"
	if (inbuff != "foobar")
		throw Exception (String("Reading from fle stream failed. Buffer=%1")
						 .arg(inbuff));

	// Read an integer (10)
	int ivalue = 0;
	in >> ivalue;
	if (ivalue != 10)
		throw Exception (String("Reading from fle stream failed; ivalue=%1").arg(ivalue));

	// Read a floating-point value (42.42)
	float fvalue = 0;
	in >> fvalue;
	if (int(fvalue*100+0.01) != int(42.42*100+0.01))
		throw Exception (String("Reading from fle stream failed; fvalue=%1").arg(fvalue));

	// Read another floating-point value (-1.2345e-10)
	in >> fvalue;
	if (fabs(fvalue - (-1.2345e-10)) > 1e-15) // Round. Can't check for exact value.
		throw Exception (String("Reading from fle stream failed; fvalue=%1").arg(fvalue));

	file.close ();
	
	/* Cleanup. */
	/*file.remove ();*/

	return true;
}

/*******************************************************************************
* NAME:        stream_stringStream
*
* DESCRIPTION: Runs TextOStream tests with a Buffer (String) device.
*
* RETURNS:     true if successful, false on failure.
*
* EXCEPTIONS:  Exception.
*******************************************************************************/
bool stream_stringStream ()
{
	/**************************************************************************/
	/*                      Explicit buffer test.                             */

	/* Use an explicitly created string buffer. */
	Buffer buffer (IO_Writable | IO_Truncate);

	/* Attach the buffer to a text stream. */
	MagiC::TextOStream out (buffer);

	/* Write stuff to the stream (and consequently to the buffer. */

	out << "beginning " << 10 << 42.42 << " end";

	/* Check that it was printed correctly. */
	if (buffer.getBuffer() != "beginning 1042.42 end")
		throw Exception (String("Writing to string stream failed. Buffer=%1")
						 .arg(buffer.getBuffer()));

	/**************************************************************************/
	/*                      Implicit buffer test.                             */

	/* Let the OStream object create the Buffer implicitly, and hidden. */
	String buf2;
	MagiC::TextOStream (buf2) << "abc+" << 42 << " def " << 42.42 << "...";
	MagiC::TextOStream (buf2, IO_Append) << "ghi" << 42 << "jkl" << 42.42 << "\n";
	MagiC::TextOStream (buf2) << "åäö";

	/* Check that it was printed correctly. */
	if (buf2 != "åäö+42 def 42.42...ghi42jkl42.42\n")
		throw Exception (String("Writing to string stream failed. Buffer=%1")
						 .arg(buf2));

	/**************************************************************************/
	/*                      Reading from string stream.                       */
	String mystr = "Line1\nLine2\n";
	MagiC::TextIStream instr1 (mystr);
	String line;
	int countin = instr1.readLine (line);
	if (countin != 6 || line != "Line1\n")
		throw Exception (String("Reading a line from string stream failed. Countin=%1").arg(countin));

	return true;
}
