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
	void animateLensTo(float targetLens, float duration, InterpolationType interpolation=LINEAR);
	ofCamera camera;
private:
	void preDraw();
	
	void onAnimationDataFloat(float value, string address, int channel);
	void updateCamPos();
	ofMesh debugMesh;
	float lens;
};

}
}

#endif // CAMERA_H
