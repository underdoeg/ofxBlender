#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "Object.h"

namespace ofx {

namespace blender {

class Billboard: public Object {
public:
	Billboard();
	~Billboard();

	virtual void preDraw();

	void setUpVector(float x, float y, float z);
	void setUpVector(ofVec3f upVec);

private:
	bool bCustomUp;
	ofVec3f customUp;
};

}

}

#endif // BILLBOARD_H
