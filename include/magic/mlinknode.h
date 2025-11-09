#ifndef __LINKNODE_H__
#define __LINKNODE_H__

#include <magic/mobject.h>
#include <magic/mpararr.h>


typedef unsigned int Word;
typedef unsigned int DWord;

typedef	Word	LnNdCnncnType ;

typedef	Word	StsLnNd ;
typedef	DWord	StsLnNB ;



//////////////////////////////////////////////////////////////////////////////
//                 |     o       |   |   |          |                       //
//                 |         _   |   |\  |          |  ___                  //
//                 |     | |/ \  | / | \ |  __   ---| /   )                 //
//                 |     | |   | |/  |  \| /  \ (   | |---                  //
//                 |____ | |   | | \ |   | \__/  ---|  \__                  //
//////////////////////////////////////////////////////////////////////////////

/** An object linked bi-directionally to other objects, inheritable.
 **/
class LinkNode {
  public:

	/** A link to a LinkNode.
	 **/
	struct NodeLink { // Was LnNdCnncnType;
		LinkNode		*node ;
		LnNdCnncnType	state ;
		char			data [0] ;
	};

	/** Array of NodeLinks.
	 **/
	class NodeLinkArray : public PackArray<NodeLink> {
	  public:
		NodeLinkArray	(IndVect sizeData = 0) : VectGrow (sizeof (NodeLink) + sizeData)	{ }
		
		/** Returns how much the array has to be grown when adding nodes. */
		virtual	IndLnNd		getGrow			(IndLnNd minSize = 1) const ;
		
		/** Finds the given node object and returns its index. */
		IndLnNd				find			(LinkNode const &node) const				{ return find (&node) ; }
		IndLnNd				find			(LinkNode const *node) const ;
	};

  public:

	LinkNode (IndVect sizeData = 0) ;
	
	virtual		~LinkNode () ;
	
	/** Creates a bi-directional connection between this and the
	 *  other node.
	 *
	 **/
	StsLnNB	connect (LinkNode const &other) ;
	StsLnNB	connect (LinkNode const *other)				{ return (other ? connect (*other) : M_stsLnNdThis (N_StsLnNd_Error | N_StsLnNd_InvalidRef)) ; }

	/** Creates a connection that reserves the other
	 *  object. Reservation implies an ownership relation (reserver
	 *  owns the reservee).
	 **/
	StsLnNd	reserve (LinkNode const &other) ;
	StsLnNd	reserve (LinkNode const *other)				{ return (other ? reserve (*other) : N_StsLnNd_Error | N_StsLnNd_InvalidRef) ; }

	/** Releases a reservation, but does not disconnect the other
	 *  object.
	 **/
	StsLnNd	release (IndLnNd iOther) ;
	StsLnNd	release (LinkNode const &other) ;
	StsLnNd	release (LinkNode const *other)				{ return (other ? release (*other) : N_StsLnNd_Error | N_StsLnNd_InvalidRef) ; }

	/** Disconnects the connection in both directions. */
	StsLnNd	disconnect (LinkNode const &other, IndLnNd iOther) ;
	StsLnNd	disconnect (LinkNode const &other) ;
	StsLnNd	disconnect (LinkNode const *other)			{ return (other ? disconnect (*other) : N_StsLnNd_Error | N_StsLnNd_InvalidRef) ; }

	/** Disconnects all connections. */
	void	disconnectAll () ;

	/** Releases all connections, but does not disconnect them. */
	void	releaseAll () ;

	/** Is the node connected to any other LinkNode. */
	Bool	isConnected (IndLnNd iOther) const			{ return (*this) [iOther].state & N_NodeLinkStt_Connected ; }
	Bool	isConnected (LinkNode const &other) const ;

	/** Is the node connected to any LinkNode belonging to the given class? */
	Bool	isConnected (ClassName clsName) const ;
	Bool	isReservedBy (IndLnNd iOther) const			{ return (*this) [iOther].state & N_NodeLinkStt_ReservedBy ; }
	Bool	isReservedBy (LinkNode const &other) const ;
	Bool	isConnected () const ;
	Bool	isReserved () const ;
	Bool	hasReserved () const ;

	/** Sets the node as protected (actually increments a protection
	 *  counter). When the node is protected, no notifications are
	 *  sent to it.
	 **/
	void	addProtect ()								{ VERIFY (++mProtected != 0) ; }

	/** Removes protection from the node (decrements a protection
	 *  counter).
	 **/
	void	removeProtect () ;

	/** Connection iteration. Returns NULL when the value is over... */
	NodeLink const	*iterConnection (LinkNodeIndex &i) const	{ return (i < items () ? itemNT (i) : NULL) ; }
	NodeLink const	*iterChanged (IndLnNd &i) const ;
	NodeLink const	*iterConnected (IndLnNd &i) const ;
	NodeLink const	*iterReserved (IndLnNd &i) const ;
	NodeLink const	*iterReserver (IndLnNd &i) const ;

	/** Finds and returns the NodeLink object connecting this node to
	 *  the other given node.
	 **/
	NodeLink const	*findConnection (LinkNode const &item) const		{ IndLnNd i = find (item) ;	return (i != N_IndLnNd_Invalid ? &((*this) [i]) : NULL) ; }

	void const	*findConnected (ClassName clsName) const				{ IndLnNd i = 0 ; return iterConnected (i, clsName) ; }
	void const	*iterConnected (IndLnNd &i, ClassName clsName) const ;
	
	NodeLinkArray const	&connections () const			{ return *this ; }
	NodeLinkType		gChgCnncn () const				{ return mChgCnncn ; }
	unsigned int		gProtected () const				{ return mProtected ; }
	NodeLink const		*gLastChanged () const			{ return mLastChanged ; }

	/** Returns the owner object for the node, in case the node is
	 *  owned as an attribute, not inherited. If it is inherited,
	 *  returns pointer to itself.
	 **/
	virtual	CPtr	getHost (ClassName className) const	{ return (className ? castClsF (className) : clsName ()) ; }
	
	Bool	mNotifyConnection ;

  protected:
	StsLnNd	removeProtect (StsLnNd ret)					{ removeProtect () ; return ret ; }
	StsLnNB	removeProtect (StsLnNB ret)					{ removeProtect () ; return ret ; }
	
  private: // Methods
	StsLnNd setConnected (LinkNode const &other) ;
	StsLnNd	setDisconnected (IndLnNd other) ;
	StsLnNd	setDisconnected (LinkNode const &other)		{ return setDisconnected (find (other)) ; }
	StsLnNd setConnectedAnd (LinkNode const &other, NodeLinkType status, NodeLinkType chg) ;
	void	setReleasedBy (IndLnNd other) ;
	void	setReleasedBy (LinkNode const &other)		{ setReleasedBy (find (other)) ; }

	void	sendNotifys () ;
	Bool	purgeDisconnectedConnections () ;			// N_true if purged
	
	void	connectionChanged (NodeLink const *cnn, NodeLinkType chg)	{ mChgCnncn |= chg ; mLastChanged = (mLastChanged ? (NodeLink *) -1 : cnn) ; }

	/** Notifies the node about... */
	virtual	void	notifyBeginNotify (NodeLinkType changed)	{ }
	
	/** Notifies the node that a connection has been added or removed,
     *  a reservation added or removed.
	 *
	 *  @param chg Type of the change.
	 *
	 *  @param cnncn The connection in question. If the node has been
	 *  disconnected, the connection is not yet removed.
	**/
	virtual	void	notifyConnection (NodeLinkType chg, NodeLink const *cnncn)	{ }

	/** Notifies the node that a connection has been purged. */
	virtual void	notifyNotifiedAndPurged (NodeLinkType chg)	{ }

  private: // Attributes
	
	NodeLinkArray mNodeLinks;
	
	NodeLinkType	mChgCnncn ;

	/** Protects the node from notifications. This makes the NodeLinks
	 *  re-entrant.
	 **/
	unsigned int	mProtected ;

	/** For optimization when only one connection has been changed.
	 *  Value -1 (sic) means that several connections has changed.
	 **/
	NodeLink const	*mLastChanged ;
};

inline VERIFY_stsLnNdIsOK (StsLnNd sts) {
	ASSERT (stsLnNdIsOK (sts)) ;
	return sts ;
}

inline VERIFY_not_stsLnNdIsError (StsLnNd sts) {
	ASSERT (!stsLnNdIsError (sts)) ;
	return sts ;
}

inline VERIFY_stsLnNdIsOK (StsLnNB sts) {
	ASSERT (stsLnNdIsOK (sts)) ;
	return sts ;
}

inline VERIFY_not_stsLnNdIsError (StsLnNB sts) {
	ASSERT (!stsLnNdIsError (sts)) ;
	return sts ;
}

#if defined (_DEBUG)
	void trace (LinkNode const &ob, IndVect indent = 0, NodeLinkType show1 = N_NodeLinkStt_Connected) ;
#endif	// defined (_DEBUG)

#ifdef __STRING_H__
	String& links2Str (String& ret, LinkNode const &ob) ;
#endif	// __STRING_H__


inline Bool	stsLnNdIsOK (StsLnNd sts)		{ return !(sts & (N_StsLnNd_Warning | N_StsLnNd_Error)) ; }
inline Bool stsLnNdIsError (StsLnNd sts)	{ return !!(sts & N_StsLnNd_Error) ; }
inline Bool	stsLnNdIsWarning (StsLnNd sts)	{ return !!(sts & N_StsLnNd_Warning) ; }

#define	M_stsLnNdThis(sts)					((StsLnNB) (StsLnNd) sts)
#define	M_stsLnNdOther(sts)					(((StsLnNB) sts) << 16)

inline Bool	stsLnNdIsOK (StsLnNB sts)		{ return !(sts & (M_stsLnNdThis (N_StsLnNd_Error | N_StsLnNd_Warning) | M_stsLnNdOther (N_StsLnNd_Error | N_StsLnNd_Warning))) ; }
inline Bool stsLnNdIsError (StsLnNB sts)	{ return !!(sts & (M_stsLnNdThis (N_StsLnNd_Error) | M_stsLnNdOther (N_StsLnNd_Error))); }
inline Bool	stsLnNdIsWarning (StsLnNB sts)	{ return !!(sts & (M_stsLnNdThis (N_StsLnNd_Warning) | M_stsLnNdOther (N_StsLnNd_Warning))) ; }

inline StsLnNd	stsLnNdThis (StsLnNB sts)					{ return (StsLnNd) sts ; }
inline StsLnNd	stsLnNdOther (StsLnNB sts)					{ return (StsLnNd) (sts >> 16) ; }
inline StsLnNB	stsLnNdThis (StsLnNd sts)					{ return M_stsLnNdThis(sts) ; }
inline StsLnNB	stsLnNdOther (StsLnNd sts)					{ return M_stsLnNdOther(sts) ; }
inline StsLnNB	stsLnNd (StsLnNd stsThis, StsLnNd stsOther)	{ return stsLnNdThis (stsThis) | stsLnNdOther (stsOther) ; }
inline StsLnNB	stsLnNdSwapThisAndOther (StsLnNB sts)		{ return stsLnNd (stsLnNdOther (sts), stsLnNdThis (sts)) ; }


// typedef IndVect				IndLnNd ;
// #define N_IndLnNd_Invalid	N_IndVect_Invalid

inline LinkNode *node (NodeLink const *cnn)	{ return (cnn ? cnn->node : NULL) ; }

#define	N_LnNdCnncnStt_Connected		0x0001
#define	N_LnNdCnncnStt_ReservedBy		0x0002	/* This node is reserved by LnNdCnncn::node. */
#define	N_LnNdCnncnStt_Reserve			0x0004	/* This node reserves LnNdCnncn::node. */
#define	N_LnNdCnncnStt_FlagA			0x0010
#define	N_LnNdCnncnStt_FlagB			0x0020
#define	N_LnNdCnncnStt_FlagC			0x0040
#define	N_LnNdCnncnStt_FlagD			0x0080
#define	N_LnNdCnncnStt_ChgReserve		0x0100
#define	N_LnNdCnncnStt_ChgRelease		0x0200
#define	N_LnNdCnncnStt_ChgReservedBy	0x0400
#define	N_LnNdCnncnStt_ChgReleasedBy	0x0800
#define	N_LnNdCnncnStt_ChgInvalidRef	0x2000
#define	N_LnNdCnncnStt_ChgConnected		0x4000
#define	N_LnNdCnncnStt_ChgDisconnected	0x8000
#define	N_LnNdCnncnStt_Chg_				0xef00
#define	N_LnNdCnncnStt_					0xeff7

#define	N_StsLnNd_OK				0x0000
#define	N_StsLnNd_Warning			0x1000
#define N_StsLnNd_Error				0x2000
#define N_StsLnNd_AlreadyDone		0x0001
#define N_StsLnNd_NoSuchLnNd		0x0002
#define N_StsLnNd_InvalidRef		0x0004
#define	N_StsLnNd_Linked			0x0008
#define	N_StsLnNd_ReleasedBy		0x0010
#define	N_StsLnNd_Release			0x0020
#define	N_StsLnNd_				   (0x0007 | N_StsLnNd_OK | N_StsLnNd_Warning | N_StsLnNd_Error)


//////////////////////////////////////////////////////////////////////////////
//      ---           | o     |     o       |   |   |          |            //
//       |    _       |       |         _   |   |\  |          |  ___       //
//       |  |/ \   ---| | |/\ |     | |/ \  | / | \ |  __   ---| /   )      //
//       |  |   | (   | | |   |     | |   | |/  |  \| /  \ (   | |---       //
//      _|_ |   |  ---| | |   |____ | |   | | \ |   | \__/  ---|  \__       //
//////////////////////////////////////////////////////////////////////////////

/** LinkNode that can be connected to just one kind of other nodes. It
 *  can be used as an attribute, not inherited. It contains just one
 *  type of objects.
 **/
template <class T>
class IndirLinkNode : public LinkNode {
  public:
	typedef CPtr	(T::*FGetHost) (ClassName) const ; 
	typedef void	(T::*FNfyBegN) (NodeLinkType) ;
	typedef	void	(T::*FNfyNfPg) (NodeLinkType) ;
	typedef	void	(T::*FNfyCnn) (NodeLinkType chg, NodeLink const *) ;
	
					IndirLinkNode () : mFunc_notifyConnection (NULL), mFunc_notifyBeginNotify (NULL), mFunc_notifyNotifiedAndPurged (NULL), mFunc_getHost (NULL)	{ }

	/** Tells the object its owner. */
	void			setOwner						(T &owner)			{ mOwner = &owner ; }

	/** */
	T*				gOwner () const										{ return mOwner ; }

	/** TODO: Must use RTTI instead!
	 *
	 *  
	 **/
	virtual	CPtr	getHost (ClassName clsName) const			{ return (clsName ? (mFunc_getHost ? (mOwner->*mFunc_getHost) (clsName) : mOwner->castClsF (clsName)) : mOwner->clsName ()) ; }

	FGetHost		setFunc_getOwner				(FGetHost func)		{ FGetHost ret = mFunc_getHost ; mFunc_getHost = func ; return ret ; }
	FNfyBegN		setFunc_notifyBeginNotify		(FNfyBegN func)		{ FNfyBegN ret = mFunc_notifyBeginNotify ; mFunc_notifyBeginNotify = func ; return ret ; }
	FNfyNfPg		setFunc_notifyNotifiedAndPurged	(FNfyNfPg func)		{ FNfyNfPg ret = mFunc_notifyNotifiedAndPurged ; mFunc_notifyNotifiedAndPurged = func ; return ret ; }
	FNfyCnn			setFunc_notifyConnection		(FNfyCnn func)		{ FNfyCnn ret = mFunc_notifyConnection ; mFunc_notifyConnection = func ; if (mFunc_notifyConnection) mNotifyConnection = N_true ; return ret ; }

	/** A quicker way to set the callback methods. */
	void			set (T &client, FGetHost func1, FNfyNfPg func2, FNfyBegN func4, FNfyCnn func3) ;

  private:
	virtual	void	notifyBeginNotify (NodeLinkType changed)						{ if (mFunc_notifyBeginNotify) (mClient->*mFunc_notifyBeginNotify) (changed) ; }
	virtual	void	notifyConnection (NodeLinkType chg, NodeLink const *cnncn)	{ if (mFunc_notifyConnection) (mClient->*mFunc_notifyConnection) (chg, cnncn) ; }
	virtual void	notifyNotifiedAndPurged (NodeLinkType chg)						{ if (mFunc_notifyNotifiedAndPurged) (mClient->*mFunc_notifyNotifiedAndPurged) (chg) ; }

	/** Owner of the node object. */
	T	*mOwner ;
	
	FNfyCnn		mFunc_notifyConnection ;
	FNfyBegN	mFunc_notifyBeginNotify ;
	FNfyNfPg	mFunc_notifyNotifiedAndPurged ;
	FGetHost	mFunc_getHost ;
};

template <class T>
void	IndirLinkNode<T>::set (T &client, FGetHost func1, FNfyNfPg func2, FNfyBegN func4, FNfyCnn func3) {
	mClient = &client ;
	mFunc_getHost = func1 ;
	mFunc_notifyNotifiedAndPurged = func2 ;
	mFunc_notifyBeginNotify = func4 ;
	mFunc_notifyConnection = func3 ;
	if (mFunc_notifyConnection)
		mNotifyConnection = N_true ;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ----                                  | |     o       |   |   |          |       |   |          o           //
// (            ____  --   ___    _       | |         _   |   |\  |          |  ___  |\  |       |     __       //
//  ---  |   | (     |  ) /   ) |/ \   ---| |     | |/ \  | / | \ |  __   ---| /   ) | \ |  __  -+- | /   \   | //
//     ) |   |  \__  |--  |---  |   | (   | |     | |   | |/  |  \| /  \ (   | |---  |  \| /  \  |  | +--  \  | //
// ___/   \__! ____) |     \__  |   |  ---| |____ | |   | | \ |   | \__/  ---|  \__  |   | \__/   \ | |     \_/ //
//                                                                                                    |    \_/  //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** A collection of LinkNodes that silences all notifications to the
 *  contained nodes.
 * 
 *  Silences are removed automatically when this object is destroyed.
 **/
class SuspendLinkNodeNotify : public LinkNode {
  public:
	SuspendLinkNodeNotify ()					{ mNotifyConnection = N_true ; }
	virtual	~SuspendLinkNodeNotify () ;
	
	void	suspend (LinkNode const &other)		{ VERIFY_not_stsLnNdIsError (connect (other)) ; }
	void	release (LinkNode const &other)		{ VERIFY_not_stsLnNdIsError (disconnect (other)) ; }
	void	releaseAll ()						{ disconnectAll () ; }
	
  protected:
	/** Catch the notifications here and silence them. */
	virtual	void	notifyConnection (NodeLinkType chg, NodeLink const *cnncn) ;
	/** Catch the notifications here and silence them. */
	virtual void	notifyNotifiedAndPurged (NodeLinkType chg) ;
};

SuspendLinkNodeNotify &operator, (SuspendLinkNodeNotify &suspend, LinkNode &connect) ;

//////////////////////////////////////////////////////////////////////////////
class ClcnNodesPrc : public LinkNode {
		M_DeclareDynClass
	public:
		virtual	CPtr	getHost (ClassName className) const	{ return (className ? NULL : _mClsName) ; }
};



//////////////////////////////////////////////////////////////////////////////
//  |   |          |       ----                                             //
//  |\  |          |  ___  |   )           ___   ___   ____  ____           //
//  | \ |  __   ---| /   ) |---  |/\  __  |   \ /   ) (     (      __  |/\  //
//  |  \| /  \ (   | |---  |     |   /  \ |     |---   \__   \__  /  \ |    //
//  |   | \__/  ---|  \__  |     |   \__/  \__/  \__  ____) ____) \__/ |    //
//////////////////////////////////////////////////////////////////////////////

/** Node batch processor...
 **/
class LinkNodeProcessor {
  public:
	LinkNodeProcessor () ;
	
	virtual			~LinkNodeProcessor		() ;

	void			process				() ;
	
	UInt			gPendingCount		() const				{ return mPendingCount ; }
	void			addSuspend			()						{ mPendingCount++ ; }
	void			removeSuspend		() ;
	Bool			hasWaitingProcesses () const				{ return mCurrPrcNodes->isConnected () ; }
	
	void			addNode				(LinkNode const &node)	{ VERIFY_not_stsLnNdIsError (pcrNodesCurr ().connect (node)) ; }
	
	ClcnNodesPrc	&prcNodesNotCurr	()						{ return *(mCurrPrcNodes == &mNodesA ? &mNodesB : (mCurrPrcNodes == &mNodesB ? &mNodesA : NULL)) ; }
	ClcnNodesPrc	&pcrNodesCurr		() const				{ return *mCurrPrcNodes ; }
	
  protected:
	UInt			mPendingCount ;
	ClcnNodesPrc	mNodesA ;
	ClcnNodesPrc	mNodesB ;
	ClcnNodesPrc	*mCurrPrcNodes ;

	/** This method is called for every link. **/
	virtual void	process (const NodeLink& cnn)	{MUST_OVERLOAD}
};

class SuspendLinkNodeProcessor {
	public:
		SuspendLinkNodeProcessor (LinkNodeProcessor *const *processors) ;
		virtual	~SuspendLinkNodeProcessor () ;
	
	protected:
		LinkNodeProcessor *const *mProcessors ;
};

//////////////////////////////////////////////////////////////////////////////
template<class T, T *prcNodes>
	class SuspendPrcNodes {
		public:
			SuspendPrcNodes ()											{ prcNodes->addSuspend () ; }
			SuspendPrcNodes (LinkNode const &sender)					{ prcNodes->addSuspend () ; _addNode (sender) ; }

			virtual	~SuspendPrcNodes ()									{ prcNodes->removeSuspend () ; }

			virtual	UInt	gPendingCount () const						{ return _gPendingCount () ; }

			static T	*_gNotifySender ()								{ return prcNodes ; }
			static UInt	_gPendingCount ()								{ return prcNodes->gPendingCount () ; }
			static void	_addNode (LinkNode const &sender)				{ prcNodes->addNode (sender) ; }
	};

//////////////////////////////////////////////////////////////////////////////
#endif // I_mlinknode.h
