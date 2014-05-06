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

void Scene::addObject(Object* obj){
    objects.push_back(obj);
    switch(obj->type){
    case MESH:
        meshes.push_back(static_cast<Mesh*>(obj));
        break;
    }
}

}

}

