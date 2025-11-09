################################################################################
#    This file is part of the MagiC++ library.                                 #
#                                                                              #
#    Copyright (C) 1998-2002 Marko Grönroos <magi@iki.fi>                      #
#                                                                              #
################################################################################
#                                                                              #
#   This library is free software; you can redistribute it and/or              #
#   modify it under the terms of the GNU Library General Public                #
#   License as published by the Free Software Foundation; either               #
#   version 2 of the License, or (at your option) any later version.           #
#                                                                              #
#   This library is distributed in the hope that it will be useful,            #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of             #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU          #
#   Library General Public License for more details.                           #
#                                                                              #
#   You should have received a copy of the GNU Library General Public          #
#   License along with this library; see the file COPYING.LIB.  If             #
#   not, write to the Free Software Foundation, Inc., 59 Temple Place          #
#   - Suite 330, Boston, MA 02111-1307, USA.                                   #
#                                                                              #
################################################################################

################################################################################
# Define root directory of the source tree
################################################################################
export SRCDIR ?= ..

################################################################################
# Define module name and compilation type
################################################################################
modname          = libmagic
compile_library  = 1
makedox          = 1
modversionfile   = lmversion.mk
modversionheader = mversion.h

################################################################################
# Include build framework
################################################################################
include $(SRCDIR)/build/magicdef.mk

################################################################################
# Source files for libmagic.a
################################################################################

sources = mstream.cc mtextstream.cc mdatastream.cc mmath.cc \
	mregexp.cc mattribute.cc mstring.cc mmap.cc \
	mmatrix.cc miodevice.cc mclass.cc mdatetime.cc mhtml.cc mobject.cc \
	mgobject.cc mgdev-eps.cc mturtle.cc mlsystem.cc mthread.cc \
	mlog.cc

shared_headers = mclass.h mstream.h mtextstream.h \
	mdatastream.h mdebug.h mlist.h mobject.h mset.h mmath.h \
	mcoord.h mdynamic.h mloop.h mpackarray.h mtable.h mmatrix.h \
	marchive.h mstring.h mhtml.h mmagisupp.h mpararr.h mregexp.h \
	mattribute.h mdatetime.h miterator.h mmap.h mrefarray.h mconfig.h \
	mparameter.h mgobject.h mgdev-eps.h mexception.h mtypes.h \
	miodevice.h mi18n.h mturtle.h mlsystem.h mthread.h merrors.h \
	mlog.h mgraph.h

headersubdir = magic

################################################################################
# Copy shared headers
################################################################################
#extra_targets = copy-shared-headers

#shared_headers_full = $(foreach file,$(shared_headers),$(grpincdir)/$(headersubdir)/$(file))
#hdrmoddir = $(incdir)/$(headersubdir)

#copy-shared-headers: $(hdrmoddir)
#	cp $(shared_headers_full) $(hdrmoddir)
#$(hdrmoddir):
#	mkdir -p $(hdrmoddir)


################################################################################
# Recursively compile some subprojects
################################################################################
makemodules = libapp test extras

################################################################################
# Compile
################################################################################
include $(SRCDIR)/build/magiccmp.mk

