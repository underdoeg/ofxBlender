#include "Scene.h"

namespace ofx {

namespace blender {

Scene::Scene() {
	name = "unnamed scene";
}

Scene::~Scene() {

}

void Scene::update() {
	timeline.step();
}

void Scene::draw() {
	for(Object* obj: objects) {
		obj->draw();
	}
}

void Scene::addObject(Object* obj) {
	objects.push_back(obj);
	switch(obj->type) {
	case MESH:
		meshes.push_back(static_cast<Mesh*>(obj));
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

}

}
