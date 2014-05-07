#ifndef BLENDER_SCENE_H
#define BLENDER_SCENE_H

#include "Object.h"
#include "Animation.h"
#include "Mesh.h"

namespace ofx {
namespace blender {

class Scene{
public:
    Scene();
    ~Scene();

    void draw();
    void update();
    void addObject(Object* obj);
	Object* getObject(unsigned int index);
	Object* getObject(string name);
	Mesh* getMesh(unsigned int index);
	Mesh* getMesh(string name);

    Timeline timeline;
	string name;
private:
    std::vector<Object*> objects;
    std::vector<Mesh*> meshes;
};

}
}

#endif // OBJECT_H
