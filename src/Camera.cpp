#include "Camera.h"

namespace ofx {
namespace blender {

Camera::Camera() {
	type = CAMERA;
	//camera.setParent(*this);

	debugMesh = ofMesh::cone(.5, 2, 6, 2, 1);
}

Camera::~Camera() {
}

void Camera::customDraw() {
	ofDrawAxis(.5);
	ofSetColor(255);
	//ofLine(0, 0, 0, 0, 0, -10);
	ofPushMatrix();
	ofRotateX(-90);
	debugMesh.drawWireframe();
	ofPopMatrix();
}

void Camera::onOrientationChanged() {
	camera.setOrientation(getOrientationQuat());
}

void Camera::onPositionChanged() {
	camera.setPosition(getPosition());
}

void Camera::updateCamPos() {
}

}
}
