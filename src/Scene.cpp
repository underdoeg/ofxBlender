#include "Scene.h"

namespace ofx {

namespace blender {

Scene::Scene() {
	name = "unnamed scene";
	activeCamera = NULL;
}

Scene::~Scene() {

}

void Scene::update() {
	timeline.step();
}

void Scene::draw() {
	if(activeCamera)
		activeCamera->camera.begin();

	for(Object* obj: objects) {
		if(obj->type != CAMERA)
			obj->draw();
	}

	if(activeCamera)
		activeCamera->camera.end();
}

void Scene::addObject(Object* obj) {
	objects.push_back(obj);
	timeline.add(&obj->timeline);
	switch(obj->type) {
	case MESH:
		meshes.push_back(static_cast<Mesh*>(obj));
		break;
	case CAMERA:
		cameras.push_back(static_cast<Camera*>(obj));
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

Camera* Scene::getActiveCamera() {
	return activeCamera;
}

}
} //end namespace
