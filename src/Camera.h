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
	void setLens(float lens);
	//void onScaleChanged();
	ofCamera camera;
private:
	void onAnimationDataFloat(float value, string address, int channel);
	void updateCamPos();
	ofMesh debugMesh;
};

}
}

#endif // CAMERA_H
