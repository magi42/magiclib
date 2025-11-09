================================================================================
$Author: magi $ $Date: 2003/12/31 14:31:17 $ $Revision: 1.1 $

MagiC++ library (MagiCLib)

Copyright (C) 1997-2003 Marko Grönroos (magi@iki.fi)

Distributed under GNU Lesser General Public License.
================================================================================

========
Contents
========

  * Introduction
  * Installation
  * Compatibility
  * Other Notes

============
Introduction
============

MagiC++ library (MagiCLib) is a lightweight C++ base class library,
released under GNU LGPL (Lesser General Public License). It's API is
in many ways similar to Qt and has actually been converging towards
Qt. However, certain essential features differentiate it from Qt:

  * The license
  * Use of multiple inheritance
  * Use of exceptions
  * No GUI stuff
  * No wide char strings
  * A nice build system, replacement for GNU Autoconf/Automake

But most importantly: it's really light-weight. It doesn't have much
else but the basic object hierarchy, basic containers, and a few
special features. Some special stuff is provided in supplementary
libraries.

---------------------------------------------------------------------
LISENCE NOTICE: While the libraries are under LGPL, any applications
                included in the package, such as any test code and
                samples, are under GNU GPL (General Public License).
---------------------------------------------------------------------

============
Installation
============

This is important!

The MagiCLib should be installed so that its include files are in directory
INCLUDEDIR/magic/*.h.

  Thus you can include the headers with, for example:

	#include <magic/object.h>

This is done as follows:

   $ tar zxf magic-1.0.tar.gz
   $ cd magic-1.0
   $ ./configure --prefix=/usr/local --includedir=/usr/local/include/magic
   $ make deps
   $ make
   $ make install

-------------
COMPATIBILITY
-------------

MagiClib currently compiles under i386 Linux.

It does not compile under Solaris, because it gets math-exception
errors. There are #ifdef guards against this, but they don't really
work currently.
	
-----------
OTHER NOTES
-----------

MagiClib is my personal base class library. The most important classes are
String, Map, Array (and its variants), List, and Table.

It provides much the same functionality as Qt, but is better, although much
much smaller.

The main reason why I don't use containers from Qt or STL:

 - Containers in Qt || STL don't "own" the objects (destroy them along
   with the container), or if they do, that behaviour can't be
   modified.

 - You can't put either references or pointer objects to the
   containers. For example:

       Array<String> myContainer;
       myContainer.add (new String ("Hello"));
       mycontainer.add (String("Hello"));

   You can do the latter in Qt, but not the former. And even if you can,
   the object will not be owned by the container.

 - Yes, that's the only reason that comes to my mind right now. But
   it's bad.
