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

	ofCamera camera;
private:
	ofMesh debugMesh;
};

}
}

#endif // CAMERA_H
