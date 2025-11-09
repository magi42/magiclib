#include <magic/mstring.h>
#include <magic/mtextstream.h>
using namespace MagiC;

bool iodevice_fileWriting ()
{
	// Open file
	File file ("/tmp/testfile.txt");

	// Write something to the file
	file.open (IO_Writable);
	String line1 = "Here is a line of text to the file\n";
	String line2 = "Here is more text to the file\n";
	file.IODevice::writeBlock (line1);
	file.IODevice::writeBlock (line2);
	file.close ();

	// Read the file, while checking that it really contains what we
	// wrote there earlier.
	
	file.open (IO_Readable);
	String linebuf;
	int lineno = 0;
	while (!file.atEnd()) {
		// Read one line
 		int readcnt = file.readLine (linebuf, 1000);

		// EOF can only happen here. Not fun.
		if (readcnt == 0)
			break;

		// Check that the line is correct.
		if (lineno == 0) {
			if (linebuf != line1)
				cout << "Error, read line differs from written line 1:\n"
					 << line1 << linebuf;
		} else if (lineno == 1) {
			if (linebuf != line2)
				cout << "Error, read line differs from written line 2:\n"
					 << line2 << linebuf;
		} else
			cout << "Error, too many lines in written file\n";

		lineno++;
	}
	file.close ();

	// Remove the file

	if (!file.exists())
		throw Exception ("File does not exist as it should.\n");
	file.remove ();
	if (file.exists())
		throw Exception ("File still exists, removing failed.\n");
	
	return true;
}

/** This tests all possible file I/O operations, such as seek, and
 *  different ways to open a file.
 **/
bool iodevice_fileOperations ()
{
	return true;
}

bool iodevice_bufferOperations ()
{
	return true;
}

