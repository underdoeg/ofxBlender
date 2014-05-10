#ifndef LIGHT_H
#define LIGHT_H

#include "Object.h"

namespace ofx
{

namespace blender
{

class Light: public Object
{
public:
	Light();
	~Light();
	void customDraw();
	ofLight light;
};

}

}

#endif // LIGHT_H
