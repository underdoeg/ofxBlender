#ifndef OBJECT_H
#define OBJECT_H

#include "Utils.h"
#include "ofMain.h"
namespace ofx
{

namespace blender
{

class Object: public ofNode
{
public:
	Object();
	~Object();
	string name;
};

}

}

#endif // OBJECT_H
