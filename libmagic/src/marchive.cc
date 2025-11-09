#include <mobject.h>
#include "magic/marchive.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////  CArchive:Object  /////////////////////////////

CArchive::CArchive (const char* filnam, int md) {
	in=NULL;
	out=NULL;
	indent=0;
	arcfile=filnam;
	mode = md;
	attribs = NULL;
	if (mode&ARC_SAVE)
		opensave();
	else
		if (mode&ARC_RECORD)
			openrecord ();
		else
			openload();
}

CArchive::~CArchive () {
	delete in;
	delete out;
	delete attribs;
}

int CArchive::open (const char* filename, int flag) {
	delete out;
	delete in;
	if (fbuf.is_open())
		fbuf.close ();

	filebuf* succ=NULL;
	String filname = arcfile;
	if (filename)
		filname = filename;
	succ = fbuf.open (filname, ios::IOSBINARY | ((flag==ARC_LOAD)? (ios::in|ios::nocreate) : ios::out));
	if (!succ || !fbuf.is_open ()) {
		// FAILBY("Could not open file for reading");
		errst = 1;
		return 0;
	}
	return 1;
}

int CArchive::opensave (const char* filename) {
	if (!open (filename, ARC_SAVE)) {
		errst = 1;
		return 0;
	}
	out = new ostream (&fbuf);
	if (!out) {
		errst = 1;
		// FAILBY ("Unnable to open output stream");
		return 0;
	}
	return 1;
}

int CArchive::openload (const char* filename) {
	if (!open (filename, ARC_LOAD)) {
		errst=1;
		return 0;
	}
	in = new istream (&fbuf);
	if (!in) {
		errst = 1;
		// FAILBY ("Unable to open input stream");
		return 0;
	}
	return 1;
}

int CArchive::openrecord () {
	attribs = new Array<String>;
}

void CArchive::close () {
	fbuf.close ();
	delete in;
	delete out;
	in = NULL;
	out = NULL;
}

#define CHECKRECORD(rtype,var) \
	if (mode&ARC_RECORD) {\
		if (nextname.isempty())\
			nextname = "?";\
		attribs->add (new String (format ("%s %s", #rtype, (CONSTR)nextname)));\
		return *this;\
	}
//		nextname += format (" = %s", (CONSTR) String (var));\

CArchive& CArchive::operator<< (int i) {
	CHECKRECORD (int, i);

	operator<< ((long) i);
	return *this;
}

CArchive& CArchive::operator<< (long i) {
	CHECKRECORD (long, int (i));

	if (mode&ARC_TEXT) {
		for (int j=0; j<indent; j++) *out << "\t";
		*out << i << "\n";
	} else
		out->write ((char*) &i, sizeof(i));
	return *this;
}

CArchive& CArchive::operator<<	(char i)				{
	CHECKRECORD (char, i);
	if (out) out->write ((char*) &i, sizeof(i)); return *this;
}

CArchive& CArchive::operator<< (float i) {
	CHECKRECORD (float, i);
	operator<< ((double) i);
	return *this;
}

CArchive& CArchive::operator<<	(double i)				{
	CHECKRECORD (double, float(i));
	if (mode&ARC_TEXT) {
		for (int j=0; j<indent; j++) *out << "\t";
		*out << i << "\n";
	} else
		out->write ((char*) &i, sizeof(i));
	return *this;
}

CArchive& CArchive::write		(const char* p, int n)	{
	CHECKRECORD (char*, p);
	if (mode&ARC_TEXT) {
		for (int i=0; i<indent; i++) *out << "\t";
		String str, str2 (p, n);
		str.hexcode (str2);
		*out << str << "\n";
	} else
		out->write (p, n);
	return *this;
}

CArchive& CArchive::operator>>	(long& i)			{
	if (in) in->read ((char*) &i, sizeof(i)); return *this;
}

CArchive& CArchive::operator>>	(char& i)			{
	if (mode&ARC_TEXT) {
		;
	} else
		in->read ((char*) &i, sizeof(i));
	return *this;
}

CArchive& CArchive::operator>>	(double& i)			{
	if (in) in->read ((char*) &i, sizeof(i)); return *this;
}
CArchive& CArchive::read		(char* p, int n)	{
	if (in) in->read (p, n); return *this;
}

CArchive& operator<< (CArchive& out, const char* str) {
	out.name (str);
	return out;
}
