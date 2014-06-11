#ifndef BLENDER_OBJECT_H
#define BLENDER_OBJECT_H

#include "Utils.h"
#include "ofMain.h"
#include "Animation.h"
#include "Constraint.h"

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
	
	void update();
	void draw();

	virtual void customDraw();

	void addChild(Object* child);
	Object* getParent();
	bool hasParent();
	bool isVisible();
	void show();
	void hide();
	void toggleVisibility();
	
	void addConstraint(Constraint* constraint);
	
	string name;
	ObjectType type;
	Timeline timeline;
	Scene* scene;
	Layer* layer;
	
	ofEvent<Object*> positionChanged;
	ofEvent<Object*> orientationChanged;
	ofEvent<Object*> scaleChanged;

protected:
	virtual void onAnimationDataFloat(float value, string address, int channel);
	virtual void onAnimationDataBool(bool value, string address, int channel);

	void onOrientationChanged();
	void onPositionChanged();
	void onScaleChanged();

private:
	Object* lookAtTarget;
	ofVec3f lookAtUp;
	bool visible;
	Object* parent;
	std::vector<Object*> children;
	std::vector<Constraint*> constraints;
};

}
}

#endif // OBJECT_H
