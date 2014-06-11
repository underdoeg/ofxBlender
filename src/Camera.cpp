#include "Camera.h"

namespace ofx {
namespace blender {

Camera::Camera() {
	type = CAMERA;
	//camera.setParent(*this);
		
	debugMesh = ofMesh::cone(1, 3, 6, 2, 1);
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
	Object::onOrientationChanged();
	camera.setOrientation(getGlobalOrientation());
}

void Camera::onPositionChanged() {
	Object::onPositionChanged();
	camera.setPosition(getGlobalPosition());
}

void Camera::updateCamPos() {
}

void Camera::setLens(float lens)
{
	float fov = ofRadToDeg(2 * atan(16 / lens));
	camera.setFov(fov);
}

void Camera::onAnimationDataFloat(float value, string address, int channel) {
	Object::onAnimationDataFloat(value, address, channel);
	
	if(address == "lens"){
		setLens(value);
	}
}

}
}

