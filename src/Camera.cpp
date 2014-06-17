#include "Camera.h"

namespace ofx {
namespace blender {

Camera::Camera() {
	type = CAMERA;
	//camera.setParent(*this);
	camera.setFarClip(800);

	debugMesh = ofMesh::cone(1, 3, 6, 2, 1);
}

Camera::~Camera() {
}

void Camera::customDraw() {
	ofDrawAxis(.5);
	ofSetColor(255);
	ofPushMatrix();
	ofRotateX(-90);
	debugMesh.drawWireframe();
	ofPopMatrix();
}

void Camera::onOrientationChanged() {
	Object::onOrientationChanged();
	camera.setOrientation(getGlobalOrientation());
}

void Camera::onPositionChanged() {
	Object::onPositionChanged();
	camera.setPosition(getGlobalPosition());
}

void Camera::updateCamPos() {
}

void Camera::setLens(float l) {
	lens = l;
	float fov = ofRadToDeg(2 * atan(16.f / (lens * 2.f)));
	camera.setFov(fov);
}

float Camera::getLens() {
	return lens;
}

void Camera::onAnimationDataFloat(float value, string address, int channel) {
	Object::onAnimationDataFloat(value, address, channel);

	if(address == "lens") {
		setLens(value);
	}
}

void Camera::animateLensTo(float targetLens, float duration, InterpolationType interpolation) {
	timeline.animateTo(getLens(), targetLens, duration, "lens", 0, interpolation);
}

}
}
