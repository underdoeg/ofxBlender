#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "Object.h"

namespace ofx
{

namespace blender
{

class Billboard: public Object
{
public:
	Billboard();
	~Billboard();
	
	virtual void preDraw();
};

}

}

#endif // BILLBOARD_H