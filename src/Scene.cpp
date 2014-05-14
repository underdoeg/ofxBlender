#include "Scene.h"

namespace ofx {

namespace blender {

Scene::Scene() {
	name = "unnamed scene";
	activeCamera = NULL;
	//setScale(10);
	doDebug = false;
	isFirstDebugEnable = true;
}

Scene::~Scene() {

}

void Scene::setDebug(bool state) {
	if(state && isFirstDebugEnable && activeCamera) {
		debugCam.setNearClip(.01);
		debugCam.setGlobalPosition(activeCamera->getGlobalPosition());
		debugCam.setGlobalOrientation(activeCamera->getGlobalOrientation());
		debugCam.setDistance(debugCam.getTarget().getGlobalPosition().distance(activeCamera->getGlobalPosition()));
		//debugCam.setTransformMatrix(activeCamera->getLocalTransformMatrix());
	}
	doDebug = state;
	isFirstDebugEnable = false;
}

void Scene::toggleDebug() {
	setDebug(!doDebug);
}

void Scene::update() {
	timeline.step();
}

void Scene::customDraw() {
	ofPushStyle();

	ofEnableDepthTest();
	ofSetSmoothLighting(true);
	ofEnableLighting();

	//camera
	if(activeCamera && !doDebug)
		activeCamera->camera.begin();
	if(doDebug)
		debugCam.begin();

	//lights
	for(Light* light: lights) {
		light->begin();
	}

	//action
	for(Object* obj: objects) {
		if(doDebug)
			obj->draw();
		else if(obj->type != CAMERA && obj->type != LIGHT)
			obj->draw();
	}

	//kill the lights
	for(Light* light: lights) {
		light->end();
	}

	//draw object names
	if(doDebug) {
		ofDisableDepthTest();
		ofDisableLighting();
		ofSetColor(255);
		for(Object* obj: objects) {
			ofDrawBitmapString(obj->name, obj->getGlobalPosition());
		}
	}

	//end the camera
	if(!doDebug && activeCamera)
		activeCamera->camera.end();
	if(doDebug)
		debugCam.end();

	ofPopStyle();
}

void Scene::addObject(Object* obj) {
	objects.push_back(obj);
	timeline.add(&obj->timeline);
	obj->setParent(*this);
	switch(obj->type) {
	case MESH:
		meshes.push_back(static_cast<Mesh*>(obj));
		break;
	case CAMERA:
		cameras.push_back(static_cast<Camera*>(obj));
		break;
	case LIGHT:
		lights.push_back(static_cast<Light*>(obj));
		break;
	default:
		break;
	}
}

//templated helper to retrieve objects
template<typename Type>
Type* getFromVecByName(std::vector<Type*> vec, string name) {
	for(Type* obj: vec) {
		if(obj->name == name)
			return obj;
	}
	return NULL;
}

template<typename Type>
Type* getFromVecByIndex(std::vector<Type*> vec, unsigned int index) {
	if(vec.size() > index) {
		return vec[index];
	}
	return NULL;
}

Object* Scene::getObject(string name) {
	return getFromVecByName<Object>(objects, name);
}

Object* Scene::getObject(unsigned int index) {
	return getFromVecByIndex<Object>(objects, index);
}

Mesh* Scene::getMesh(string name) {
	return getFromVecByName<Mesh>(meshes, name);
}

Mesh* Scene::getMesh(unsigned int index) {
	return getFromVecByIndex<Mesh>(meshes, index);
}

Camera* Scene::getCamera(string name) {
	return getFromVecByName<Camera>(cameras, name);
}

Camera* Scene::getCamera(unsigned int index) {
	return getFromVecByIndex<Camera>(cameras, index);
}

void Scene::setActiveCamera(Camera* cam) {
	activeCamera = cam;
}

void Scene::disableCamera() {
	setActiveCamera(NULL);
}

Camera* Scene::getActiveCamera() {
	return activeCamera;
}

Light* Scene::getLight(string name) {
	return getFromVecByName<Light>(lights, name);
}

Light* Scene::getLight(unsigned int index) {
	return getFromVecByIndex<Light>(lights, index);
}

}
} //end namespace
