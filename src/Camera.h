#ifndef CAMERA_H
#define CAMERA_H

#include "Object.h"

namespace ofx {
namespace blender {

class Camera: public ofx::blender::Object {
public:
	Camera();
	~Camera();

	void customDraw();
	void onPositionChanged();
	void onOrientationChanged();
	//void onScaleChanged();
	ofCamera camera;
private:
	void updateCamPos();
	ofMesh debugMesh;
};

}
}

#endif // CAMERA_H
