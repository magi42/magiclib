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

#include <stdio.h>
#include <time.h>
#include <ctype.h>

#include "magic/mmath.h"
#include "magic/mdatetime.h"
#include "magic/mclass.h"

BEGIN_NAMESPACE (MagiC);

decl_module (datetime);

impl_dynamic (DateTime, {"Object"});
impl_dynamic (JulianDay, {"Object"});

///////////////////////////////////////////////////////////////////////////////
// Local functions

#define sign(x)	((x)>0)? 1:(((x)<0)? -1:0)

#define FERR 0.000000001

FloatType torange (FloatType initial, FloatType rmin, FloatType rmax) {
	while (initial<rmin || initial>rmax) {
		if (initial<rmin)
			initial += rmax-rmin;
		if (initial>rmax)
			initial -= rmax-rmin;
	}
	return initial;
}

String Hours::toString () const {
	return format ("%dh %dm %2.4fs", hour(), min(), sec());
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                 ___                   ----- o                             //
//                 |  \   ___   |   ___    |            ___                  //
//                 |   |  ___| -+- /   )   |   | |/|/| /   )                 //
//                 |   | (   |  |  |---    |   | | | | |---                  //
//                 |__/   \__|   \  \__    |   | | | |  \__                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

void settimezone (DateTime* dt) {
	// apufunktio: Asetetaan timezone ja kesäaika
	time_t aika_s = time (0);
	struct tm* aika = localtime (&aika_s);
	dt->timezone = -timezone/3600;
	dt->dst = aika->tm_isdst;
	dt->utcdiff = dt->timezone + dt->dst;
}

DateTime::DateTime (int d, int mo, int y, int h, int mi, FloatType s) {
	make (d, mo, y, h, mi, s);
}

void DateTime::make (int d, int mo, int y, int h, int mi, FloatType s) {
	day = d;
	month = mo;
	year = y;
	hours = h + mi/60.0 + s/3600.0;
	settimezone (this);
}

void DateTime::make () {
	day = 0;
	month = 0;
	year = 0;
	hours = 0.0;
	settimezone (this);
}

void DateTime::make (const time_t& tt) {
	struct tm* time_tm = localtime (&tt);
	day		= time_tm->tm_mday;
	month	= time_tm->tm_mon+1;
	year	= time_tm->tm_year+1900;
	hours	= time_tm->tm_hour
			+ time_tm->tm_min/60.0
			+ time_tm->tm_sec/3600.0;
	settimezone (this);
}

DateTime& DateTime::makecurrent () {
	make (time (0));
	return *this;
}

int DateTime::daynumber () const {
	int leap=0;
	if ((year/4)*4==year && (year/100)*100!=year)
		leap=1;

	if (month <3)
		return int ((month-1)*(63-leap)/2)+day;
	return int ((month+1)*30.6001)-63+leap+day;
}

int DateTime::weekday () const {
	return 0;
}

void DateTime::make (const JulianDay& jd) {
	double	JD = jd+0.5;
	int		Z = int(JD);
	double	F = JD-Z;
	double	A, B, C, D, E;

	if (Z<2299161) 
		A = Z;
	else {
		double alpha = int ((Z-1867216.25)/36524.25);
		A = Z+1+alpha-int(alpha/4);
	}
	
	B = A+1524;
	C = int ((B-122.1)/365.25);
	D = int (365.25*C);
	E = int ((B-D)/30.6001);
	day = int(int (B-D-int (30.6001*E))+F);
	month = int ((E<14)? E-1 : E-13);
	year = int ((month>2)? C-4716 : C-4715);

	// Lasketaan vielä kellonaika
	hours = F*24;
}

DateTime& DateTime::operator= (const DateTime& o) {
	memcpy (this, &o, sizeof(DateTime));
	return *this;
}

int DateTime::setstrict (const String& str, const String& tformat) {
	int mins=0, secs=0;

	make ();
	
	if (str.length() != tformat.length())
		return 1;
	
	for (uint i=0; i<str.length(); i++) {
		char num = str[i] - '0';
		char f = tformat[i];
		switch (f) {
		  case 'Y':	year = year*10 + num; break;
		  case 'M':	month = month*10 + num; break;
		  case 'D':	day = day*10 + num; break;
		  case 'h':	hours = hours*10 + num; break;
		  case 'm':	mins = mins*10 + num; break;
		  case 's':	secs = secs*10 + num; break;
		};
	}

	hours += mins/60.0 + secs/3600.0;

	return 0;
}

int DateTime::compare (const DateTime& other) const {
	if (year < other.year)		return -1;
	if (year > other.year)		return 1;

	if (month < other.month)	return -1;
	if (month > other.month)	return 1;

	if (day < other.day)		return -1;
	if (day > other.day)		return 1;

	if (hours < other.hours)	return -1;
	if (hours > other.hours)	return 1;

	return 0;
}

double DateTime::operator- (const DateTime& o) const {
	return FloatType(JulianDay (*this)) - FloatType(JulianDay (o));
}

DateTime DateTime::lt2GMT () const {
	DateTime result;
	result.make (day,month,year,0,0,0);
	result.hours = hours - Hours(utcdiff);
	result.corrange ();
	return result;
}

void DateTime::nextday () {
	day=day+1;
	if (day>28 && month==2)
		day = 1,
		month++;
	else
		if (day>30 && (month==4 || month==6 || month==9 || month==11))
			day = 1,
			month++;
		else
			if (day>31 && (month==1 || month==3 || month==5 || month==7
						   || month==8 || month==10 || month==12))
				day = 1,
				month++;
}

void DateTime::prevday () {
	day=day-1;
	if (day<1) {
		month--;
		if (month<1)
			year--,
			month=12;
		if (month==2)
			day=28;
		else
			if (month==4 || month==6 || month==9 || month==11)
				day=30;
			else
				if (month==1 || month==3 || month==5 || month==7
					|| month==8 || month==10 || month==12)
					day=31;
	}
}

void DateTime::corrange () {
	while (hours<0)
		hours+=24,
		prevday();
	while (hours>24)
		hours-=24,
		nextday();
}

String DateTime::text () const {
	String res = format ("%02d.%02d.%04d %02d:%02d:%02.2g",
						 day, month, year, hour(), min(), sec());
	return res;
}

String DateTime::text_time () const {
	String res = format ("%02d:%02d:%02.2g", hour(), min(), sec());
	return res;
}

String DateTime::text_time (const String& form) const {

	struct tm* tms = to_tm ();

	//TRACE3 ("%d.%d.%d", tms->tm_mday, tms->tm_mon, tms->tm_year);
	char buffer [80];
	strftime (buffer, 80, (CONSTR) form, tms);

	delete tms;

	return buffer;
}

int DateTime::isduring (const TimePeriod& period) const {
	return operator> (period.start) && operator< (period.end);
}

typedef struct tm tmstr;

struct tm* DateTime::to_tm () const {
	struct tm* result = (struct tm*) new tmstr;
	result->tm_sec	= int (sec ());
	result->tm_min	= min ();
	result->tm_hour	= int (hours);
	result->tm_mday	= day;
	result->tm_mon	= month-1;
	result->tm_year	= year-1900;
	result->tm_wday	= 0;
	result->tm_yday	= 0;
	result->tm_isdst= dst;
	return result;
}



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                  --       | o             ___                             //
//                   |       |    ___    _   |  \   ___                      //
//                   | |   | | |  ___| |/ \  |   |  ___| \   |               //
//               |   | |   | | | (   | |   | |   | (   |  \  |               //
//                \_/   \__! | |  \__| |   | |__/   \__|   \_/               //
//                                                        \_/                //
///////////////////////////////////////////////////////////////////////////////

double _JD (int day, int month, int year) {
	ASSERT (month!=0);

	if (month<=2)
		year = year-1,
		month = month+12;

	double	b = 0.0; // If after Oct 15. 1582
	if (year > 1582 || (year==1582 && (month>10 || (month==10 && day>=15)))) {
		double a = int (year/100.0);
		b = 2 - a + int (a/4.0);
	}

	return int (365.25*(year+4716)) + int ((month+1)*30.6001) + day + b - 1524.5;
}

void JulianDay::make (const DateTime& dt) {
	mJD = _JD (dt.day,dt.month,dt.year) + dt.dectime();
}

END_NAMESPACE;
