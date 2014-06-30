#ifndef BLENDER_SCENE_H
#define BLENDER_SCENE_H

#include "Object.h"
#include "Animation.h"
#include "Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "Layer.h"

namespace ofx {
namespace blender {

class Scene: public ofNode{
public:
    Scene();
    ~Scene();

	void setDebug(bool state);
	void toggleDebug();
	bool isDebugEnabled();

    void customDraw();
    void update();
    void addObject(Object* obj);
	bool hasObject(Object* obj);
	Object* getObject(unsigned int index);
	Object* getObject(string name);
	Mesh* getMesh(unsigned int index);
	Mesh* getMesh(string name);
	std::vector<Mesh*> getMeshes();
	Camera* getCamera(string name);
	Camera* getCamera(unsigned int index);
	Light* getLight(string name);
	Light* getLight(unsigned int index);
	void setLightningEnabled(bool state);
	
	Camera* getActiveCamera();
	ofCamera* getDebugCamera();
	void setActiveCamera(Camera* cam);
	void disableCamera();
	
	void setViewport(float x, float y, float w, float h);
	ofRectangle& getViewport();
	bool hasViewport();
	
	ofFloatColor bgColor;

    Timeline timeline;
	string name;
	std::map<int, Layer> layers;
	ofEasyCam debugCam;
	
private:
	void onWindowResize(ofResizeEventArgs& args);
	
	Camera* activeCamera;
    std::vector<Object*> objects;
    std::vector<Mesh*> meshes;
	std::vector<Camera*> cameras;
	std::vector<Light*> lights;
	std::vector<Material*> materials;
	bool bHasViewport;
	ofRectangle viewport;
	bool doDebug;
	bool doLightning;
	bool isFirstDebugEnable;
};

}
}

#endif // OBJECT_H
