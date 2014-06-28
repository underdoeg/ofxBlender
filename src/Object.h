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

struct VecDouble{
	double x;
	double y;
	double z;
};

class Object: public ofNode {
public:
	Object();
	~Object();
	
	void update();
	void draw(Scene* scene=NULL);

	virtual void customDraw();

	void addChild(Object* child);
	std::vector<Object*> getChildren();
	Object* getParent();
	bool hasParent();
	bool isVisible();
	void show();
	void hide();
	void toggleVisibility();
	
	void addConstraint(Constraint* constraint);
	
	void animateTo(Object* obj, float time, InterpolationType interpolation=LINEAR);
	void animatePositionTo(ofVec3f pos, float time, InterpolationType interpolation=LINEAR);
	void animateRotationTo(ofVec3f rot, float time, InterpolationType interpolation=LINEAR);
	void animateRotationTo(ofQuaternion rot, float time, InterpolationType interpolation=LINEAR);
	void animateScaleTo(ofVec3f scale, float time, InterpolationType interpolation=LINEAR);
	
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
	virtual void onAnimationDataVec3f(ofVec3f vec, string address, int channel);
	virtual void onAnimationDataQuat(ofQuaternion quat, string address, int channel);

	void onOrientationChanged();
	void onPositionChanged();
	void onScaleChanged();
	
	void onTimelinePreFrame(Timeline*&);
	void onTimelinePostFrame(Timeline*&);
	
private:
	ofVec3f originalRotation;
	Object* lookAtTarget;
	ofVec3f lookAtUp;
	bool visible;
	Object* parent;
	std::vector<Object*> children;
	std::vector<Constraint*> constraints;
	
	//helpers for euler rotation
	bool animIsEuler;
	ofVec3f eulerRot;
};

}
}

#endif // OBJECT_H
