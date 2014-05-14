#ifndef BLENDER_SCENE_H
#define BLENDER_SCENE_H

#include "Object.h"
#include "Animation.h"
#include "Mesh.h"
#include "Camera.h"
#include "Light.h"

namespace ofx {
namespace blender {

class Scene: public ofNode{
public:
    Scene();
    ~Scene();

	void setDebug(bool state);
	void toggleDebug();

    void customDraw();
    void update();
    void addObject(Object* obj);
	Object* getObject(unsigned int index);
	Object* getObject(string name);
	Mesh* getMesh(unsigned int index);
	Mesh* getMesh(string name);
	Camera* getCamera(string name);
	Camera* getCamera(unsigned int index);
	Light* getLight(string name);
	Light* getLight(unsigned int index);

	Camera* getActiveCamera();
	void setActiveCamera(Camera* cam);
	void disableCamera();

    Timeline timeline;
	string name;
private:
	Camera* activeCamera;
	ofEasyCam debugCam;
    std::vector<Object*> objects;
    std::vector<Mesh*> meshes;
	std::vector<Camera*> cameras;
	std::vector<Light*> lights;
	bool doDebug;
	bool isFirstDebugEnable;
};

}
}

#endif // OBJECT_H
