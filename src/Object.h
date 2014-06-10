#ifndef BLENDER_OBJECT_H
#define BLENDER_OBJECT_H

#include "Utils.h"
#include "ofMain.h"
#include "Animation.h"

namespace ofx {
namespace blender {

enum ObjectType {
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

	virtual void customDraw();

	void addChild(Object* child);
	Object* getParent();
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

protected:
	virtual void onAnimationDataFloat(float value, string address, int channel);
	virtual void onAnimationDataBool(bool value, string address, int channel);

private:
	bool visible;
	Object* parent;
	std::vector<Object*> children;
};

}
}

#endif // OBJECT_H
