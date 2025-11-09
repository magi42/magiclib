/** @file mi18n.h
 *  @brief Internationalization (i18n) support.
 *
 *  The internationalization scheme used in MagiC++ library is based
 *  on the GNU gettext library. The scheme can be outlined as follows:
 * 
 *    @li The xgettext program must be ran for all source code that
 *    uses the i18n, to generate the translation templates.
 *
 *    @li Then, the localization (translations) for each language must be
 *    done.
 *
 *    @li The proper run-time translation databases must be generated
 *    from the translated files and placed to proper location,
 *    typically to \c /usr/share/locale/<locale>/LC_MESSAGES directory.
 **/

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

#ifndef __MAGIC_MI18N_H__
#define __MAGIC_MI18N_H__

#ifndef QT

namespace MagiC {

	/** Translates a string to current locale.
	 *
	 *  @note The GNU gettext preparation must have been done properly for
	 *  the sources, and the translation files must be located at correct
	 *  location.
	 *
	 *  @return Returns a translated version of the given string. If no
	 *  translation is available for the current language, the same string
	 *  is returned unchanged.
	 **/
	inline String i18n (const char* text /**< Text string to translate. */)
	{
		return String (text);
	}
}


#endif

#endif
