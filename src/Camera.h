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
	float getLens();
	void updateLens();
	void updateCamPos();
	void animateLensTo(float targetLens, float duration, InterpolationType interpolation=LINEAR);
	void interpolateLensTo(Camera* cam, float t);
	void interpolateTo(Camera* cam, float t);
	ofCamera camera;
private:
	void preDraw();
	void onAnimationDataFloat(float value, string address, int channel);
	
	ofMesh debugMesh;
	float lens;
};

}
}

#endif // CAMERA_H
