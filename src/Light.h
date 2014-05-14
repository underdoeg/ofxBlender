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
	void begin();
	void end();
	void customDraw();
	ofLight light;
	//float constantAtt;
	//float linearAtt;
	//float quadraticAtt;
};

}

}

#endif // LIGHT_H
