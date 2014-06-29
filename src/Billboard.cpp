#include "Billboard.h"
#include "Scene.h"

namespace ofx {

namespace blender {

Billboard::Billboard() {
}

Billboard::~Billboard() {
}

void Billboard::customDraw() {
	if(!scene)
		return;
	ofCamera* cam = NULL;
	if(scene->isDebugEnabled())
		cam = scene->getDebugCamera();
	else if(scene->getActiveCamera())
		cam = &scene->getActiveCamera()->camera;
	if(cam)
		lookAt(cam->getGlobalPosition(), cam->getUpDir());
}

}
}
