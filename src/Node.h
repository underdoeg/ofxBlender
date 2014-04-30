#ifndef NODE_H
#define NODE_H

#include "ofNode.h"

namespace ofx
{

namespace blender
{

class Node: public ofNode
{
public:
	Node();
	~Node();

	string name;
};

}

}

#endif // NODE_H
