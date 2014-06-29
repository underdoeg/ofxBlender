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
	if(cam){
		ofVec3f up = cam->getUpDir();
		//up.z *= -1;
		lookAt(cam->getGlobalPosition(), up);
		//rotate(180, cam->getUpDir());
	}
//
}

}
}
