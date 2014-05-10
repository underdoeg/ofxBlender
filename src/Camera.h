#ifndef CAMERA_H
#define CAMERA_H

#include "Object.h"

namespace ofx {
namespace blender {

class Camera: public Object {
public:
	Camera();
	~Camera();

	void customDraw();

	ofCamera camera;
};

}
}

#endif // CAMERA_H
