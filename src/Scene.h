#ifndef BLENDER_SCENE_H
#define BLENDER_SCENE_H

#include "Object.h"
#include "Animation.h"
#include "Mesh.h"

namespace ofx {

namespace blender {

class Scene: public ofx::blender::Object {
public:
    Scene();
    ~Scene();

    void update();
    void addObject(Object* obj);

    Timeline timeline;
private:
    std::vector<Object*> objects;
    std::vector<Mesh*> meshes;
};

}

}

#endif // OBJECT_H
