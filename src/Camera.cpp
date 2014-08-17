#include "Camera.h"
#include "Scene.h"

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

void Camera::preDraw() {
	setLens(lens);
	//updateCamPos();
}

void Camera::customDraw() {
	ofDrawAxis(.5);
	ofSetColor(255);
	ofPushMatrix();
	ofRotateX(-90);
	debugMesh.drawWireframe();
	ofPopMatrix();
	//cout << name << ": " << getGlobalPosition() << endl;
}

void Camera::onOrientationChanged() {
	//Object::onOrientationChanged();
	//camera.setOrientation(getGlobalOrientation());
}

void Camera::onPositionChanged() {
	//Object::onPositionChanged();
	//camera.setPosition(getGlobalPosition());
}

void Camera::updateCamPos() {
	camera.setTransformMatrix(getGlobalTransformMatrix());
}

void Camera::setLens(float l) {
	if(l == lens)
		return;
	lens = l;
	updateLens();
}

void Camera::updateLens(){
	float w = 1920;
	float h = 1080;
	
	if(scene){
		if(scene->hasViewport()){
			w = scene->getViewport().width;
			h = scene->getViewport().height;
		}
	}
	
	float factor = lens / 32.0f;
	float pr = float(w) / float(h);
	//float fovx = atan(.5/factor);
	float fovy = atan(.5f/pr/factor);
	float fov = fovy * 360 /PI;
	camera.setFov(fov);
}

float Camera::getLens() {
	return lens;
}

void Camera::interpolateLensTo(Camera* cam, float t){
	setLens(getLens() + (cam->getLens() - getLens())*t);
}

void Camera::interpolateTo(Camera* cam, float t){
	Object::interpolateTo(cam, t);
	interpolateLensTo(cam, t);
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

