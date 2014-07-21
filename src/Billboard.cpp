#include "Billboard.h"
#include "Scene.h"

namespace ofx {

namespace blender {

Billboard::Billboard() {
}

Billboard::~Billboard() {
}

void Billboard::preDraw() {
	if(!scene)
		return;
	ofCamera* cam = NULL;
	if(scene->isDebugEnabled())
		cam = scene->getDebugCamera();
	else if(scene->getActiveCamera())
		cam = &scene->getActiveCamera()->camera;
	if(cam) {
		ofVec3f up = customUp;
		if(!bCustomUp)
			up = cam->getUpDir();
		lookAt(cam->getGlobalPosition(), up);
	}



//
}

void Billboard::setUpVector(ofVec3f upVec) {
	bCustomUp = true;
	customUp = upVec;
}

void Billboard::setUpVector(float x, float y, float z) {
	setUpVector(ofVec3f(x, y, z));
}

}
}
