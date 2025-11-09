#ifndef __MAGIC_DATETIME_H__
#define __MAGIC_DATETIME_H__

#include "magic/mobject.h"

BEGIN_NAMESPACE (MagiC);

typedef double FloatType;
typedef FloatType TimeHours;

/**
   Time in hours.
*/
class Hours {
	double	mHours;
  public:
						Hours			() : mHours (0.0) {}
						Hours			(double hours) : mHours (hours) {}
						Hours			(int hours, int mins=0, double secs=0.0)
								: mHours (((hours<0 || mins<0 || secs<0)?-1:1)*(abs(hours)+mins/60.0+secs/3600)) {}
	//						Hours			(const Seconds& secs);
	//						Hours			(const Radians& rads) : mHours (rads.toDouble()*(12/M_PI)) {}
	inline double		toDouble		() const {return mHours;}
	inline				operator double () const {return mHours;}
	inline void			operator +=		(double x) {mHours += x;}
	inline void			operator -=		(double x) {mHours -= x;}
	inline Hours		operator -		() const {return -mHours;}
	inline Hours		operator -		(const Hours& o) const {return mHours-o.mHours;}
	inline Hours		operator +		(const Hours& o) const {return mHours+o.mHours;}
	String				toString		() const;
	int					hour			() const {return int(mHours);}
	int					min				() const {return int((mHours-int(mHours))*60);}
	double				sec				() const {return ((mHours-hour())*60-min())*60;}
};
inline Hours operator * (double x, const Hours& y) {return Hours(x*y.toDouble());}

// Ensures that the given value is in the given range
FloatType torange (FloatType initial, FloatType rmin, FloatType rmax);

// Date and time conversions

class DateTime;
class TimePeriod;
class JulianDay;

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                 ___                   ----- o                             //
//                 |  \   ___   |   ___    |            ___                  //
//                 |   |  ___| -+- /   )   |   | |/|/| /   )                 //
//                 |   | (   |  |  |---    |   | | | | |---                  //
//                 |__/   \__|   \  \__    |   | | | |  \__                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

typedef DateTime Date;
typedef DateTime Time;

// This object represents any time or date value
class DateTime : public Object {
	decl_dynamic (DateTime);
  public:
	int	year, month, day;	// P‰iv‰m‰‰r‰
	Hours hours;			// Aika tunteina ja sen osina
	int	timezone;			// Mik‰ timezone
	int utcdiff;			// timezone+dst
	int	dst;				// Onko kes‰aika

					DateTime	() {make ();}
					DateTime	(const time_t& aika) {make (aika);}
					DateTime	(const struct tm* aika);
					DateTime	(const JulianDay& jd) {make(jd);}
					DateTime	(const DateTime& o) {operator=(o);}
					DateTime	(int day, int month, int year, int h=0, int m=0, FloatType s=0.0);
	void			make		();
	void			make		(const time_t& aika);
	void			make		(const JulianDay& jd);
	void			make		(int day, int month, int year, int h=0, int m=0, FloatType s=0.0);
	DateTime&		makecurrent	();

	// Mutatorit
	
	DateTime&		operator=	(const JulianDay& jd) {make(jd); return *this;}
	DateTime&		operator=	(const DateTime& o);

	// Asettaa ajan muotoilun mukaan.<BR>
	// Y=vuosi, M=kuukausi, D=p‰iv‰, h=tunti, m=minuutti, s=sekuntti.<BR>
	// Palauttaa virhetilaneessa !=0
	// Esim: "YYYYMMDDhhmmss".<BR>
	// Huom! Formaatti saa nykyisell‰‰n sis‰lt‰‰ vain n‰it‰ merkkej‰.
	int				setstrict	(const String& str, const String& format);

	void			setZone		(int zone) {timezone=zone; utcdiff=zone;}

	// Informatiifiset funktiot
	
	int				operator<	(const DateTime& o) const {return compare(o)==-1;}
	int				operator==	(const DateTime& o) const {return compare(o)==0;}
	int				operator>	(const DateTime& o) const {return compare(o)==1;}
	int				compare		(const DateTime& o) const;
	
	double			operator-	(const DateTime& o) const;
	int				weekday		() const;
	String			weekdayname	(int language=0) const;
	String			monthname	(int language=0) const;
	int				daynumber	() const;
	Hours			dectime		() const {return hours/24.0;}
	int				hour		() const {return int(hours);}
	int				min			() const {return int((double(hours)-int(double(hours)))*60);}
	FloatType		sec			() const {return (hours*60-int(hours*60))*60;}

	// Time conversions
	DateTime		lt2GMT		() const;
	DateTime		GMT2lt		(int timezone) const;

	// Tells if the time period contains this time
	int				isduring	(const TimePeriod& period) const;

	// Muunnos c-perusmuotoon. Muista deletoida.
	struct tm*		to_tm		() const;
	
	// Printing functions
	
	String			text		() const;
	String			text_time	() const;
	String			text_time	(const String& timeformat) const;

	// Iterator functions
	
	void			nextday		();
	void			prevday		();

	void			corrange	();
  private:
};



//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//             ----- o             ----            o          |             //
//               |            ___  |   )  ___                 |             //
//               |   | |/|/| /   ) |---  /   ) |/\ |  __   ---|             //
//               |   | | | | |---  |     |---  |   | /  \ (   |             //
//               |   | | | |  \__  |      \__  |   | \__/  ---|             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

// Stores a time interval
class TimePeriod : public Object {
  public:
	DateTime	start, end;

				TimePeriod	() {;}
				TimePeriod	(const DateTime& s, const DateTime& e) {start=s; end=e;}
};



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                  --       | o             ___                             //
//                   |       |    ___    _   |  \   ___                      //
//                   | |   | | |  ___| |/ \  |   |  ___| \   |               //
//               |   | |   | | | (   | |   | |   | (   |  \  |               //
//                \_/   \__! | |  \__| |   | |__/   \__|   \_/               //
//                                                        \_/                //
//////////////////////////////////////////////////////////////////////////////

class JulianDay : public Object {
	decl_dynamic (JulianDay);
	FloatType	mJD;
  public:
					JulianDay			() {mJD=0.0;}
					JulianDay			(const DateTime& dt) {operator=(dt);}
					JulianDay			(const FloatType& fl) {mJD = fl;}
	void			make				(const DateTime& dt);
	JulianDay&		operator=			(const DateTime& dt) {make (dt); return *this;}
					operator FloatType	() const {return mJD;}
};

END_NAMESPACE;

#endif



