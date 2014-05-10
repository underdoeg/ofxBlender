#include "Camera.h"

namespace ofx {
namespace blender {

Camera::Camera() {
	type = CAMERA;
	camera.setParent(*this);
}

Camera::~Camera() {
}

void Camera::customDraw() {
	camera.begin();
}

}
}
