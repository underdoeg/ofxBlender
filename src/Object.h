#ifndef BLENDER_OBJECT_H
#define BLENDER_OBJECT_H

#include "Utils.h"
#include "ofMain.h"
#include "Animation.h"

namespace ofx {
namespace blender {

enum ObjectType{
    UNDEFINED,
    MESH,
    LIGHT,
	CAMERA,
	TEXT
};

class Scene;
class Layer;

class Object: public ofNode {
public:
    Object();
    ~Object();
	
	void draw();

	void addChild(Object* child);
	bool hasParent();
	bool isVisible();
	void show();
	void hide();
	void toggleVisibility();
	
	string name;
    ObjectType type;
	Timeline timeline;
	Scene* scene;
	Layer* layer;

private:
	bool visible;
	Object* parent;
	std::vector<Object*> children;
	void onAnimationData(float value, string address, int channel);
};

}
}

#endif // OBJECT_H
