#ifndef __SET_H__
#define __SET_H__

#include <mobject.h>

class AnySet : public Object {
};



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                     |   |           o                                     //
//                     |   |  ___         ___    _    |                      //
//                     |   |  ___| |/\ |  ___| |/ \  -+-                     //
//                      \ /  (   | |   | (   | |   |  |                      //
//                       V    \__| |   |  \__| |   |   \                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

template <class ttype>
class Variant : public AnySet {
  public:
	ttype	i;

							operator ttype	() const {return i;}
	//virtual FormatOStream&	operator>>	(FormatOStream& out) const {MUST_OVERLOAD}
};

template<class ttype>
class IteratorInterface {
  public:
	virtual void	start		()=0;
	virtual void	next		()=0;
	virtual bool	exhausted	() const=0;
	virtual bool	next_ends	() const {MUST_OVERLOAD}
};


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//           -----                                                          //
//           |       _                ___       ___   |                     //
//           |---  |/ \  |   | |/|/| /   ) |/\  ___| -+-  __  |/\           //
//           |     |   | |   | | | | |---  |   (   |  |  /  \ |             //
//           |____ |   |  \__! | | |  \__  |    \__|   \ \__/ |             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


template <class ttype>
class Iterator : public Variant<ttype>, public IteratorInterface<ttype> {
	ttype	m_start, m_end, m_step;
  public:
	
				Iterator	(ttype s, ttype e=-9999, ttype stp=1) {
					m_start = s;
					ASSERTWITH (s<=e, "Start of a variant must be less than end");
					m_end = (e==-9999)? s:e;
					ASSERTWITH (stp>0, "Finite variant violation");
					m_step = stp;
					start ();
				}
	void		start		() {i = m_start;}
	void		next		() {i += m_step;}
	bool		exhausted	() const {return i>m_end;}
	bool		next_ends	() const {return i+m_step>m_end;}
	ttype		max			() const {return m_end;}
//	
//	OStream&	operator>>	(OStream& o) {
//		o.print ("{");
//		if (m_start != m_end)
//			; else
//				o << i;
//		o.print ("}");
//	}
//	FormatOStream&	operator>>	(FormatOStream& out) const {
//		out.name ("m_start") << m_start;
//		out.name ("m_end") << m_end;
//		out.name ("m_step") << m_step;
//	}
//	
};

/*
template<class maintype, class subtype>
class Iterator {//: public IteratorInterface<Array<TYPE> > {
	//	mutable Array<TYPE>&	mArray;
	//Iterator<int>				mIter;
  public:
	Iterator	(const maintype& array) //:
//						mArray (const_cast<Array<TYPE>&>(array)),
		//						mIter(array.lower, array.upper, 1) {}
				{}
	void		start		() {}//mIter.start();}
	void		next		() {}//mIter.next();}
	bool		exhausted	() const {return false;}//mIter.exhausted();}
	//				operator const ttype& () const {return mArray[mIter.i];}
};
*/



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//                ---                       o                                //
//                 |    _          ___         ___    _    |                 //
//                 |  |/ \  |   |  ___| |/\ |  ___| |/ \  -+-                //
//                 |  |   |  \ /  (   | |   | (   | |   |  |                 //
//                _|_ |   |   V    \__| |   |  \__| |   |   \                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// A "variant" that does not change (i.e., a constant)
template <class ttype>
class Invariant : public Variant<ttype> {
  public:
				Invariant	(ttype s) {
					i = s;
				}
	FormatOStream&	operator>>	(FormatOStream& out) const {
		out.name ("i") << i;
	}
};



#define forSet(name) for (name.start(); !name.exhausted(); name.next())

#endif
