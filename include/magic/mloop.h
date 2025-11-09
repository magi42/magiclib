#ifndef __LOOP_H__
#define __LOOP_H__

#include "magic/mobject.h"

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                           |                                              //
//                           |                --                            //
//                           |      __   __  |  )                           //
//                           |     /  \ /  \ |--                            //
//                           |____ \__/ \__/ |                              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

class AnyLoop : public Object {
};

template <class ttype>
class TypedLoop : public AnyLoop {
  public:
	ttype	i;

						operator ttype	() const {return i;}
	virtual FormatOStream&	operator>>	(FormatOStream& out) const=0;
};

template <class ttype>
class Loop : public TypedLoop<ttype> {
	ttype	m_start, m_end, m_step;
  public:
				Loop	(ttype s, ttype e, ttype stp=1) {
					m_start = s;
					m_end = e;
					m_step = stp;
					start ();
				}
	void		start		() {i = m_start;}
	void		next		() {i += m_step;}
	bool		exhausted	() const {return (m_step>0)? i>m_end:i<m_end;}
	bool		next_ends	() const {return (m_step>0)? i+m_step>m_end:i+m_step<m_end;}
	ttype		max			() const {return m_end;}
	FormatOStream&	operator>>	(FormatOStream& out) const {
		out.name ("m_start") << m_start;
		out.name ("m_end") << m_end;
		out.name ("m_step") << m_step;
	}
};

template <class ttype>
class NoLoop : public TypedLoop<ttype> {
  public:
				NoLoop	(ttype s) {
					i = s;
				}
	FormatOStream&	operator>>	(FormatOStream& out) const {
		out.name ("i") << i;
	}
};

#define ForLoop(name) for (name.start(); !name.exhausted(); name.next())



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                               ----                                        //
//                              (      ___   |                               //
//                               ---  /   ) -+-                              //
//                                  ) |---   |                               //
//                              ___/   \__    \                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

template <class ttype>
class Variant : public Object {
  public:
	ttype	i;

						operator ttype	() const {return i;}
	virtual FormatOStream&	operator>>	(FormatOStream& out) const=0;
};

template <class ttype>
class DynaSet : public Variant<ttype> {
	ttype	m_start, m_end, m_step;
  public:

				DynaSet	(ttype s, ttype e=-666666, ttype stp=1) {
					m_start = s;
					m_end = (e==-666666)? s:e;
					m_step = stp;
					start ();
				}
	void		start		() {i = m_start;}
	void		next		() {i += m_step;}
	bool		exhausted	() const {return (m_step>0)? i>m_end:i<m_end;}
	bool		next_ends	() const {return (m_step>0)? i+m_step>m_end:i+m_step<m_end;}
	ttype		max			() const {return m_end;}
	FormatOStream&	operator>>	(FormatOStream& out) const {
		out.name ("m_start") << m_start;
		out.name ("m_end") << m_end;
		out.name ("m_step") << m_step;
	}

	ttype		getStart	() const {return m_start;}
	ttype		getEnd		() const {return m_end;}
	ttype		getStep		() const {return m_step;}
};

#define ForSet(name) for (name.start(); !name.exhausted(); name.next())



#endif
