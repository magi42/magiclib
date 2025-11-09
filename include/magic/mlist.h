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

#ifndef __MAGIC_LIST_H__
#define __MAGIC_LIST_H__

template <class T>
class ListNode {
  public:
	T			mData;		// Actual data object
	ListNode*	mPrevNext;	// Previous and next nodes, with addresses XORed

				ListNode	(T data) {mData=data; mPrevNext=NULL;}
				~ListNode	() {/*delete mData;*/} // Observe that we can't delete the rest of list from here
};

template <class T> class ListIter;

/** Virtually double-linked list (using XOR address encoding) */
template <class T>
class List {
  public:
					List		() {mFirst=mLast=NULL;}
					List		(const List<T>& orig) {
						mFirst=mLast=NULL;
						copy (orig);
					}
					~List		() {empty();}
	
	void			add			(T object) {
		if (mLast) { // Append to tail
			ListNode<T>* newNode = new ListNode<T> (object);
			mLast->mPrevNext = (ListNode<T>*) (WORD(mLast->mPrevNext) ^ WORD(newNode));
			newNode->mPrevNext = mLast;
			mLast = newNode;
		} else // First node
			mFirst = mLast = new ListNode<T> (object);
	}
	//void			add			(const T& obj) {add (new T (obj));}

	ListNode<T>*	getFirst	() {return mFirst;}
	ListNode<T>*	getLast		() {return mLast;}

	void			empty		() {
		for (ListNode<T>* i=mFirst, *prev=NULL; i; ) {
			ListNode<T>* cur = i; // A temporary
			// Iterate here, not in for(;;), because we don't want to use a deleted node.
			i = (ListNode<T>*) (WORD(prev) ^ WORD(i->mPrevNext));
			prev = cur;
			delete cur;	// And now delete
		}
		mFirst=mLast=NULL;
	}

	void			copy		(const List<T>& orig) {
		empty ();
	}

	/** Moves all items from the given list to this list */
	void			moveItemsFrom	(List<T>& orig) {
		empty ();
		mFirst = orig.mFirst;
		mLast = orig.mLast;
		orig.mFirst = orig.mLast = NULL;
	}
	
  protected:
	ListNode<T>		*mFirst, *mLast;

	friend class ListIter<T>;
};

template <class T>
class ListIter {
  public:
					ListIter		(List<T>& list, bool backward=false)
							: mList (list) {backward? last():first();}
					ListIter		(const List<T>& list, bool backward=false)
							: mList (const_cast<List<T>&>(list)) {backward? last() : first();}
					ListIter		(const ListIter<T>& orig)
							: mList(orig.mList), mCurrent(orig.mCurrent), mPrevious (orig.mPrevious) {}
	void			first			() {mCurrent=mList.getFirst(); mPrevious=NULL;}
	void			last			() {
		mCurrent = mList.getLast();
		if (mCurrent)
			mPrevious = mCurrent->mPrevNext; // Trivial as next is NULL
		else
			mPrevious = NULL;
	}
	void			next			() {
		if (mCurrent) {
			ListNode<T>* cur = mCurrent;
			mCurrent = (ListNode<T>*) (WORD(mPrevious) ^ WORD(mCurrent->mPrevNext));
			mPrevious = cur;
		} else
			first ();
	}
	void			previous		() {
		ListNode<T>* cur = mCurrent;
		mCurrent = mPrevious;
		if (mCurrent)
			mPrevious = (ListNode<T>*)(int(cur) ^ WORD(mCurrent->mPrevNext));
		else
			mPrevious = NULL;
	}
	bool			exhausted		() const {return !mCurrent;}
	T&				get				() {return mCurrent->mData;}
	T*				getp			() {return &mCurrent->mData;}
	void			deleteCurrent	() {
		if (!mCurrent)
			return;
		
		ListNode<T>* nextNode = (ListNode<T>*) (WORD(mPrevious) ^ WORD(mCurrent->mPrevNext));
		// Adjust links of the surrounding nodes
		if (nextNode) {
			ListNode<T>* nextnextNode = (ListNode<T>*) (WORD(mCurrent) ^ WORD(nextNode->mPrevNext));
			nextNode->mPrevNext = (ListNode<T>*) (WORD(nextnextNode) ^ WORD(mPrevious));
		} else // We are at last node
			mList.mLast = mPrevious;
			
		if (mPrevious) {
			ListNode<T>* prevprevNode = (ListNode<T>*) (WORD(mCurrent) ^ WORD(mPrevious->mPrevNext));
			mPrevious->mPrevNext = (ListNode<T>*) (WORD(prevprevNode) ^ WORD(nextNode));
		} else // We are at first node
			mList.mFirst = nextNode;

		delete mCurrent;
		mCurrent = nextNode;
	}
  protected:
	List<T>&		mList;
	ListNode<T>*	mCurrent;
	ListNode<T>*	mPrevious;
};

#endif
