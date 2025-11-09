/***************************************************************************
    copyright            : (C) 2006 by Marko Grönroos
    email                : magi@iki.fi
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 *
 **/

#ifndef __MAGIC_MGRAPH_H__
#define __MAGIC_MGRAPH_H__

#include <magic/mobject.h>

class Node {
  protected:
};

template <class LabelType>
class LabeledNode : public Node {
	void				setLabel	(const LabelType& newLabel) {mLabel = newLabel;}
	LabelType&			label		() {return mLabel;}
	const LabelType&	label		() const {return mLabel;}

  protected:
	LabelType mLabel;
};

typedef LabeledNode<int> NumberedNode;

template <class NodeType>
class Edge {
	void			setSource	(NodeType* node) {mpSource = node;}
	void			setTarget	(NodeType* node) {mpTarget = node;}
	NodeType&		source		() {return *mpSource;}
	const NodeType&	source		() const {return *mpSource;}
	NodeType&		target		() {return *mpTarget;}
	const NodeType&	target		() const {return *mpTarget;}
	
  protected:
	NodeType* mpSource;
	NodeType* mpTarget;
};

template <class NodeType, class EdgeType>
class Graph {
  public:

							Graph	() {}
	virtual					~Graph	() {}
	Array<NodeType>&		nodes	() {return mNodes;}
	const Array<NodeType>&	nodes	() const {return mNodes;}
	Array<EdgeType>&		edges	() {return mEdges;}
	const Array<EdgeType>&	edges	() const {return mEdges;}

  protected:
	Array<NodeType> mNodes;
	Array<EdgeType> mEdges;
};

typedef Graph<NumberedNode, Edge<NumberedNode> > NumberedGraph;

#endif
