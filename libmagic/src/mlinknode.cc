#include "stdafx.h"
#include "mlinknode.h"

//////////////////////////////////////////////////////////////////////////////
M_ImplementDynClass (LinkNode)

LinkNode::LinkNode (IndVect sizeData) : VectLnNdCnncn (sizeData), mChgCnncn (0), mProtected (0), mNotifyConnection (N_false), mLastChanged (NULL) {
}

/*virtual*/ LinkNode::~LinkNode () {
	if (items ())
		disconnectAll () ;
}

StsLnNB	LinkNode::connect (LinkNode const &other) {
	ASSERT (&other != this) ;
	addProtect () ;
	((LinkNode &) other).addProtect () ;
	StsLnNB ret = stsLnNd (setConnected (other), ((LinkNode &) other).setConnected (*this)) ;
	((LinkNode &) other).removeProtect () ;
	removeProtect () ;
	return ret ;
}

StsLnNd	LinkNode::disconnect (LinkNode const &other, IndLnNd iOther) {
	LnNdCnncn const &cnn = (*this) [iOther] ;
	if (!(cnn.state & N_LnNdCnncnStt_Connected))
		return N_StsLnNd_Warning | N_StsLnNd_AlreadyDone ;
	addProtect () ;
	((LinkNode &) other).addProtect () ;
	IndLnNd iThis = other.find (*this) ;
	if (iOther == N_IndLnNd_Invalid || iThis == N_IndLnNd_Invalid) {
		TRACE ("Trying disconnect %p:%s:%d and %p:%s:%d.\n", (CPtr) this, (CStr) getHost (NULL), (int) iOther, (CPtr) &other, (CStr) other.getHost (NULL), (int) iThis) ;
		trace (*this, 2, N_LnNdCnncnStt_) ;
		trace (other, 2, N_LnNdCnncnStt_) ;
		assert (0) ;
	}
	VERIFY_not_stsLnNdIsError (setDisconnected (iOther)) ;
	VERIFY_not_stsLnNdIsError (((LinkNode &) other).setDisconnected (iThis)) ;
	((LinkNode &) other).removeProtect () ;
	removeProtect () ;
	return N_StsLnNd_OK ;
}

StsLnNd LinkNode::disconnect (LinkNode const &other) {
	IndVect iOther = find (other) ;
	if (iOther == N_IndVect_Invalid)
		return N_StsLnNd_Warning | N_StsLnNd_NoSuchLnNd ;
	return disconnect (other, iOther) ;
}

StsLnNd LinkNode::reserve (LinkNode const &other) {
	ASSERT (&other != this) ;
	addProtect () ;
	((LinkNode &) other).addProtect () ;
	StsLnNd ret = setConnectedAnd (other, N_LnNdCnncnStt_Reserve, N_LnNdCnncnStt_ChgReserve) ;
	((LinkNode &) other).setConnectedAnd (*this, N_LnNdCnncnStt_ReservedBy, N_LnNdCnncnStt_ChgReservedBy) ;
	((LinkNode &) other).removeProtect () ;
	removeProtect () ;
	return ret ;
}

StsLnNd	LinkNode::release (LinkNode const &other) {
	IndLnNd i = find (other) ;
	if (i == N_IndLnNd_Invalid)
		return N_StsLnNd_Error | N_StsLnNd_NoSuchLnNd ;
	return release (i) ;
}

StsLnNd LinkNode::release (IndLnNd iOther) {
	addProtect () ;
	LnNdCnncn &cnn = operator[] (iOther) ;
	if ((cnn.state & (N_LnNdCnncnStt_Reserve | N_LnNdCnncnStt_Connected)) != (N_LnNdCnncnStt_Reserve | N_LnNdCnncnStt_Connected))
		return N_StsLnNd_Warning | N_StsLnNd_AlreadyDone ;
	LinkNode *other = (LinkNode *) cnn.node ;
	other->addProtect () ;
	cnn.state &= ~N_LnNdCnncnStt_Reserve ;
	cnn.state |= N_LnNdCnncnStt_ChgRelease ;
	connectionChanged (&cnn, N_LnNdCnncnStt_ChgRelease) ;
	other->setReleasedBy (*this) ;
	other->removeProtect () ;
	removeProtect () ;
	return N_StsLnNd_OK ;
}

void LinkNode::disconnectAll () {
	addProtect () ;
	LnNdCnncn const *cnn ;
	for (IndLnNd i = 0 ; !!(cnn = iterConnection (i)) ; i++) {
		if (cnn->node && (cnn->state & N_LnNdCnncnStt_Connected))
			disconnect (*cnn->node, i) ;
	}
	removeProtect () ;
}

Bool LinkNode::isConnected (LinkNode const &other) const {
	IndLnNd i = find (other) ;
	if (i == N_IndLnNd_Invalid)
		return N_false ;
	return isConnected (i) ;
}

Bool LinkNode::isConnected (ClassName clsName) const {
	for (IndLnNd i = 0 ; iterConnected (i, clsName) != NULL ; i++)
		return N_true ;
	return N_false ;
}

Bool LinkNode::isReservedBy (LinkNode const &other) const {
	IndLnNd i = find (other) ;
	if (i == N_IndLnNd_Invalid)
		return N_false ;
	return isReservedBy (i) ;
}

Bool LinkNode::isConnected () const {
	for (IndLnNd i = 0 ; i < items () ; i++)
		if ((*this) [i].state & N_LnNdCnncnStt_Connected)
			return N_true ;
	return N_false ;
}

Bool LinkNode::isReserved () const {
	for (IndLnNd i = 0 ; i < items () ; i++)
		if ((*this) [i].state & N_LnNdCnncnStt_ReservedBy) {
			ASSERT ((*this) [i].state & N_LnNdCnncnStt_Connected) ;
			return N_true ;
		}
	return N_false ;
}

Bool LinkNode::hasReserved () const {
	for (IndLnNd i = 0 ; i < items () ; i++)
		if ((*this) [i].state & N_LnNdCnncnStt_Reserve) {
			ASSERT ((*this) [i].state & N_LnNdCnncnStt_Connected) ;
			return N_true ;
		}
	return N_false ;
}

LnNdCnncn const *LinkNode::iterChanged (IndLnNd &i) const {
	for (LnNdCnncn const *ret ; !!(ret = iterConnection (i)) ; i++)
		if (ret->state & N_LnNdCnncnStt_Chg_)
			return ret ;
	return NULL ;
}

LnNdCnncn const *LinkNode::iterConnected (IndLnNd &i) const {
	for (LnNdCnncn const *ret ; !!(ret = iterConnection (i)) ; i++)
		if (ret->state & N_LnNdCnncnStt_Connected)
			return ret ;
	return NULL ;
}

LnNdCnncn const *LinkNode::iterReserved (IndLnNd &i) const {
	for (LnNdCnncn const *ret ; !!(ret = iterConnection (i)) ; i++)
		if ((ret->state & (N_LnNdCnncnStt_Connected | N_LnNdCnncnStt_Reserve)) == (N_LnNdCnncnStt_Connected | N_LnNdCnncnStt_Reserve))
			return ret ;
	return NULL ;
}

LnNdCnncn const *LinkNode::iterReserver (IndLnNd &i) const {
	for (LnNdCnncn const *ret ; !!(ret = iterConnection (i)) ; i++)
		if ((ret->state & (N_LnNdCnncnStt_Connected | N_LnNdCnncnStt_ReservedBy)) == (N_LnNdCnncnStt_Connected | N_LnNdCnncnStt_ReservedBy))
			return ret ;
	return NULL ;
}

void const *LinkNode::iterConnected (IndLnNd &i, ClassName clsName) const {
	CPtr ret ;
	for (LnNdCnncn const *cnn ; !!(cnn = iterConnected (i)) ; i++)
		if (!!(ret = cnn->node->getHost (clsName)))
			return ret ;
	return NULL ;
}

StsLnNd LinkNode::setConnected (LinkNode const &other) {
	ASSERT (mProtected) ;	// Must be protected
	IndLnNd i ;
	LnNdCnncn *cnn ;
	if ((i = find (other)) != N_IndLnNd_Invalid) {	// Connection already exists?
		cnn = &((*this) [i]) ;
		if (cnn->state & N_LnNdCnncnStt_Connected)
			return N_StsLnNd_Warning | N_StsLnNd_AlreadyDone ;
		cnn->state |= (N_LnNdCnncnStt_Connected | N_LnNdCnncnStt_ChgConnected) ;
	} else {	// ..no, create a new connection.
		i = allocNewItem () ;
		ASSERT (i != N_IndLnNd_Invalid) ;
		cnn = &((*this) [i]) ;
		cnn->node = &((LinkNode &) other) ;
		cnn->state = (N_LnNdCnncnStt_Connected | N_LnNdCnncnStt_ChgConnected) ;
	}
	connectionChanged (cnn, N_LnNdCnncnStt_Connected | N_LnNdCnncnStt_ChgConnected) ;
	return N_StsLnNd_OK ;
}

StsLnNd LinkNode::setConnectedAnd (LinkNode const &other, LnNdCnncnType status, LnNdCnncnType change) {
	ASSERT (mProtected) ;	// Must be protected
	IndLnNd i ;
	LnNdCnncn *cnn ;
	LnNdCnncnType chg ;
	if ((i = find (other)) != N_IndLnNd_Invalid) {	// Connection already exists?
		cnn = &((*this) [i]) ;
		if ((cnn->state & status) == status) {
			ASSERT (cnn->state & N_LnNdCnncnStt_Connected) ;
			return N_StsLnNd_Warning | N_StsLnNd_AlreadyDone ;
		}
		if (!(cnn->state & N_LnNdCnncnStt_Connected)) {
			cnn->state |= (N_LnNdCnncnStt_Connected | N_LnNdCnncnStt_ChgConnected) ;
			chg = N_LnNdCnncnStt_Connected | N_LnNdCnncnStt_ChgConnected ;
		} else
			chg = 0 ;
	} else {	// ..no, create a new connection.
		i = allocNewItem () ;
		ASSERT (i != N_IndLnNd_Invalid) ;
		cnn = &((*this) [i]) ;
		cnn->node = &((LinkNode &) other) ;
		cnn->state = (N_LnNdCnncnStt_Connected | N_LnNdCnncnStt_ChgConnected) ;
		chg = N_LnNdCnncnStt_Connected | N_LnNdCnncnStt_ChgConnected ;
	}
	chg |= status | change ;
	cnn->state |= chg ;
	connectionChanged (cnn, chg) ;
	return N_StsLnNd_OK ;
}

StsLnNd LinkNode::setDisconnected (IndLnNd iOther) {
	ASSERT (mProtected) ;	// Must be protected
	if (iOther == N_IndLnNd_Invalid || items () < iOther)
		return N_StsLnNd_Error | N_StsLnNd_InvalidRef ;
	LnNdCnncn &cnn = operator[] (iOther) ;
	if (!(cnn.state & N_LnNdCnncnStt_Connected))
		return N_StsLnNd_Warning | N_StsLnNd_AlreadyDone ;
	StsLnNd ret = N_StsLnNd_OK ;
	LnNdCnncnType chg = 0 ;
	if (cnn.state & N_LnNdCnncnStt_ReservedBy) {	// Is this node reserved?
		chg |= N_LnNdCnncnStt_ChgReleasedBy ;
		ret |= N_StsLnNd_ReleasedBy ;
	}
	if (cnn.state & N_LnNdCnncnStt_Reserve) {
		chg |= N_LnNdCnncnStt_ChgRelease ;
		ret |= N_StsLnNd_Release ;
	}
	cnn.state &= ~(N_LnNdCnncnStt_Connected | N_LnNdCnncnStt_Reserve | N_LnNdCnncnStt_ReservedBy | N_LnNdCnncnStt_FlagA | N_LnNdCnncnStt_FlagB | N_LnNdCnncnStt_FlagC | N_LnNdCnncnStt_FlagD) ;
	chg |= N_LnNdCnncnStt_ChgDisconnected ;
	cnn.state |= chg ;
	connectionChanged (&cnn, chg) ;
	return ret ;
}

void LinkNode::setReleasedBy (IndLnNd iOther) {
	ASSERT (mProtected) ;	// Must be protected
	ASSERT (iOther != N_IndLnNd_Invalid && iOther < items ()) ;
	LnNdCnncn &cnn = operator[] (iOther) ;
	ASSERT ((cnn.state & (N_LnNdCnncnStt_ReservedBy | N_LnNdCnncnStt_Connected)) == (N_LnNdCnncnStt_ReservedBy | N_LnNdCnncnStt_Connected)) ;	// Reserved and connected.
	cnn.state &= ~N_LnNdCnncnStt_ReservedBy ;
	cnn.state |= N_LnNdCnncnStt_ChgReleasedBy ;
	connectionChanged (&cnn, N_LnNdCnncnStt_ChgReleasedBy) ;
}

void LinkNode::removeProtect () {
	ASSERT (mProtected != 0) ;
	if (--mProtected == 0 && mChgCnncn) {
		notifyBeginNotify (mChgCnncn) ;

		// Execute loop as long as the connections of the node are
		// changed somehow during the notifys
		LnNdCnncnType changed = 0 ;
		while (mChgCnncn) {
			ASSERT (mLastChanged) ;
			changed |= mChgCnncn ;
			mProtected++ ;
			mChgCnncn = 0 ;
			sendNotifys () ;
			VERIFY (--mProtected == 0) ;
		}

		// If any connection has been marked as disconnected, purge
		// them now
		if (changed & N_LnNdCnncnStt_ChgDisconnected)
			purgeDisconnectedConnections () ;

		// Notify the node that disconnected links have been purged
		notifyNotifiedAndPurged (changed) ;
	}
}

void LinkNode::sendNotifys () {
	LnNdCnncn *cnn = (LnNdCnncn *) mLastChanged ;
	mLastChanged = NULL ;
	if (items ())
		if (mNotifyConnection) {	// Send notify about changed connections?
			// Is only one connection changed?
			if (cnn != (LnNdCnncn *) -1) {
				// yes, we can do it in O(1) by using the saved
				// connection reference
				LnNdCnncnType state = cnn->state ;
				cnn->state &= ~N_LnNdCnncnStt_Chg_ ;
				notifyConnection (state, cnn) ;
			} else {
				// no, several connections changed so iterate through
				// all connections.
				IndLnNd i = items () - 1 ;
				do {
					LnNdCnncn &cnn = (*this) [i] ;
					if (cnn.node)
						if (cnn.state & N_LnNdCnncnStt_Chg_) {
							LnNdCnncnType state = cnn.state ;
							cnn.state &= ~N_LnNdCnncnStt_Chg_ ;
							notifyConnection (state, &cnn) ;
						}
				} while (i-- != 0) ;
			}
		} else	// No, just clear changed-flag.
			for (IndLnNd i = 0 ; i < items () ; i++)
				(*this) [i].state &= ~N_LnNdCnncnStt_Chg_ ;
}

inline Bool isConnectionUsed (LnNdCnncnType state) {
	return (state & N_LnNdCnncnStt_Connected) ;
}

Bool LinkNode::purgeDisconnectedConnections () {	// Purge not used connections.
	if (items ()) {
		Bool ret = N_false ;
		IndVect size = items () ;
		Bool purge = N_false ;
		for (IndLnNd i = 0 ; i < items () && isConnectionUsed ((*this) [i].state) ; i++) ;	// Search connection to move.
		if (i < size) {	// We encountered to unused connection.
			IndLnNd iMoveTo = i ;
			do {
				for (i++ ; i < items () && !isConnectionUsed ((*this) [i].state) ; i++) ;	// Search a connected connection.
				if (i < items ()) {
					IndLnNd iMoveFrom = i ;
					for (i++ ; i < items () && isConnectionUsed ((*this) [i].state) ; i++) ;
					memmove (itemNT (iMoveTo), itemNT (iMoveFrom), (i - iMoveFrom) * mItemSize) ;
					size = iMoveTo = iMoveTo + (i - iMoveFrom) ;
				} else
					size = iMoveTo ;
			} while (i < items ()) ;
			setSize (size) ;
			return N_true ;
		}
	}
	return N_false ;
}

void trace (LinkNode const &ob, IndVect indent, LnNdCnncnType show1) {
	String fill ;
	if (indent) {
		fill.set (' ', indent) ;
		TRACE ((CStr) fill) ;
	}
	TRACE ("%p: [%d/%d] %d %04lx %s %s ", (CPtr) &ob, (int) ob.connections ().items (), (int) ob.connections ().allocated (), (int) ob.gProtected (), (DWord) ob.gChgCnncn (), ob.clsName (), (CStr) ob.getHost (NULL)) ;
	char str [100] ;
	TRACE (ob.gLastChanged () ? (ob.gLastChanged () == (LnNdCnncn const *) -1 ? "-1" : type2Str (str, ob.connections ().index (ob.gLastChanged ()))) : "NULL") ;
	TRACE ("\n") ;
	LnNdCnncn const	*cnn ;
	for (IndLnNd i = 0 ; (cnn = ob.iterConnection (i)) != NULL ; i++)
		if (show1 == N_LnNdCnncnStt_ || (cnn->state & show1)) {
			if (indent)
				TRACE ((CStr) fill) ;
			TRACE ("    [%d] (%04lx %p %s %s)\n", (int) i, (DWord) cnn->state, (CPtr) cnn->node, (CStr) cnn->node->clsName (), (CStr) cnn->node->getHost (NULL)) ;
		}
}

#ifdef __STRING_H__
	String& links2Str (String& ret, LinkNode const &ob) {
		char str [100] ;
		sprintf (str, "%p: %d [%d/%d] %04lx %s %s ", (CPtr) &ob, (int) ob.gProtected (), (int) ob.connections ().items (), (int) ob.connections ().allocated (), (DWord) ob.gChgCnncn (), ob.clsName (), (CStr) ob.getHost (NULL)) ;
		ret = str ;
		ret += (ob.gLastChanged () ? (ob.gLastChanged () == (LnNdCnncn const *) -1 ? "-1" : type2Str (str, ob.connections ().index (ob.gLastChanged ()))) : "NULL") ;
		LnNdCnncn const	*cnn ;
		for (IndLnNd i = 0 ; (cnn = ob.iterConnected (i)) != NULL ; i++) {
			sprintf (str, " (%4lx %p %s %s)", (DWord) cnn->state, (CPtr) cnn->node, (CStr) cnn->node->clsName (), (CStr) cnn->node->getHost (NULL)) ;
			ret += str ;
		}
		return ret ;
	}
#endif	// __STRING_H__

//////////////////////////////////////////////////////////////////////////////
/*virtual*/IndLnNd	VectLnNdCnncn::getGrow (IndLnNd minSize) const {
	IndLnNd ret = max ((IndLnNd) minSize, (IndLnNd) (mItems + (mItems ? mItems : 5))) ;
	return ret ;
}

IndLnNd VectLnNdCnncn::find (LinkNode const *node) const {
	if (items ()) {
		IndLnNd i = items () - 1 ;
		do
			if (operator[] (i).node == node)
				return i ;
		while (i-- != 0) ;
	}
	return N_IndLnNd_Invalid ;
}



////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  ----                                  | |     o       |   |   |          |       |   |          o           //
// (            ____  --   ___    _       | |         _   |   |\  |          |  ___  |\  |       |     __       //
//  ---  |   | (     |  ) /   ) |/ \   ---| |     | |/ \  | / | \ |  __   ---| /   ) | \ |  __  -+- | /   \   | //
//     ) |   |  \__  |--  |---  |   | (   | |     | |   | |/  |  \| /  \ (   | |---  |  \| /  \  |  | +--  \  | //
// ___/   \__! ____) |     \__  |   |  ---| |____ | |   | | \ |   | \__/  ---|  \__  |   | \__/   \ | |     \_/ //
//                                                                                                    |    \_/  //
////////////////////////////////////////////////////////////////////////////////

/*virtual*/ SuspendLinkNodeNotify::~SuspendLinkNodeNotify () {
	releaseAll () ;
}

void SuspendLinkNodeNotify::notifyConnection (LnNdCnncnType chg, LnNdCnncn const *cnncn) {
	if ((chg & (N_LnNdCnncnStt_Connected | N_LnNdCnncnStt_ChgConnected)) == (N_LnNdCnncnStt_Connected | N_LnNdCnncnStt_ChgConnected)) {				// Linked new node?
		LinkNode *node = (LinkNode *) cnncn->node ;
		ASSERT (node) ;
		node->addProtect () ;
	} else if ((chg & (N_LnNdCnncnStt_Connected | N_LnNdCnncnStt_ChgDisconnected)) == N_LnNdCnncnStt_ChgDisconnected) {	// Unlinked node?
		LinkNode *node = (LinkNode *) cnncn->node ;
		ASSERT (node) ;
		node->removeProtect () ;
	}
}

void SuspendLinkNodeNotify::notifyNotifiedAndPurged (LnNdCnncnType chg) {
}

SuspendLinkNodeNotify &operator, (SuspendLinkNodeNotify &suspend, LinkNode &connect) {
	VERIFY_not_stsLnNdIsError (suspend.connect (connect)) ;
	return suspend ;
}
	


//////////////////////////////////////////////////////////////////////////////
//  |   |          |       ----                                             //
//  |\  |          |  ___  |   )           ___   ___   ____  ____           //
//  | \ |  __   ---| /   ) |---  |/\  __  |   \ /   ) (     (      __  |/\  //
//  |  \| /  \ (   | |---  |     |   /  \ |     |---   \__   \__  /  \ |    //
//  |   | \__/  ---|  \__  |     |   \__/  \__/  \__  ____) ____) \__/ |    //
//////////////////////////////////////////////////////////////////////////////

//M_ImplementDynClass1 (ClcnNodesPrc, LinkNode)

ProcessNodes::ProcessNodes () {
	mCurrPrcNodes = &mNodesA ;
	mPendingCount = 0 ;
}

ProcessNodes::~ProcessNodes () {
}

void ProcessNodes::removeSuspend () {
	if (--mPendingCount == 0)
		process () ;
}

void ProcessNodes::process () {
	while (pcrNodesCurr ().isConnected ()) {
		mPendingCount++ ;
		LinkNode &senders = pcrNodesCurr () ;
		mCurrPrcNodes = &prcNodesNotCurr () ;	// Change notify-sender.

		senders.addProtect () ;
		LnNdCnncn const *cnn ;
		for (IndLnNd i = 0 ; !!(cnn = senders.iterConnected (i)) ; i++)
			process (*cnn) ;
		senders.disconnectAll () ;
		senders.removeProtect () ;
		mPendingCount-- ;
	}
	ASSERT (!mNodesA.isConnected ()) ;
	ASSERT (!mNodesB.isConnected ()) ;
}

SuspendProcessNodes::SuspendProcessNodes (ProcessNodes *const *processors) : mProcessors (processors) {
	for (char i = 0 ; mProcessors [i] ; i++)
		mProcessors [i]->addSuspend () ;
}

/*virtual*/ SuspendProcessNodes::~SuspendProcessNodes () {
	ASSERT (mProcessors [0]) ;
	char iPrc ;
	ProcessNodes *prc ;
	if (mProcessors [0]->gPendingCount () == 1) {	// This is highest level suspender?
		char i ;
		for (iPrc = 0 ; !!(prc = mProcessors [iPrc]) ; iPrc++) {
			ASSERT (prc->gPendingCount () == 1) ;
			prc->process () ;
			for (i = 0 ; i < iPrc ; i++)				// Check if removeSuspend caused new processes for lower level(s).
				if (mProcessors [i]->hasWaitingProcesses ())
					iPrc = i - i ;
		}
	}
	for (iPrc = 0 ; !!(prc = mProcessors [iPrc]) ; iPrc++)
		prc->removeSuspend () ;
}

//////////////////////////////////////////////////////////////////////////////
// (c) Wallac Oy & Instrudev Oy & Mao 86
//
// Created:	1994-1997
// Modifications:
// ........	...	...	.........................................................
// 19991122		Mao	Rewritten in Wallac Oy. Based on LinkBase LINK.H & LINK.CPP
/////////////////////////////////////////////////////////////////////////////
