#ifndef BLENDER_OBJECT_H
#define BLENDER_OBJECT_H

#include "Utils.h"
#include "ofMain.h"
#include "Animation.h"
#include "Constraint.h"

namespace ofx {
namespace blender {

enum ObjectType {
    EMPTY,
    MESH,
    LIGHT,
    CAMERA,
    TEXT
};

class Scene;
class Layer;

struct VecDouble {
	double x;
	double y;
	double z;
};

struct ObjectEventArgs {
	Object* obj;
};

class Object: public ofNode {
public:

	Object();
	~Object();

	void update();
	void draw(Scene* scene=NULL, bool drawChildren=true);

	virtual void preDraw() {};
	virtual void customDraw();
	virtual void postDraw() {};

	void addChild(Object* child, bool keepGlobalTransform=false);
	void removeChild(Object* child);
	bool hasChild(Object* obj);

	std::vector<Object*> getChildren();
	Object* getParent();
	bool hasParent();
	bool isVisible();
	void setVisible(bool state);
	void show();
	void hide();
	void toggleVisibility();

	void addConstraint(Constraint* constraint);

	void interpolateTo(Object* obj,  float t);
	void animateTo(Object* obj, float time, InterpolationType interpolation=LINEAR);
	void animatePositionTo(ofVec3f pos, float time, InterpolationType interpolation=LINEAR);
	void animateRotationTo(ofVec3f rot, float time, InterpolationType interpolation=LINEAR);
	void animateRotationTo(ofQuaternion rot, float time, InterpolationType interpolation=LINEAR);
	void animateScaleTo(ofVec3f scale, float time, InterpolationType interpolation=LINEAR);

	ofVec2f getPositionOnScreen(ofRectangle viewport = ofGetCurrentViewport());

	string name;
	ObjectType type;
	Timeline timeline;
	Scene* scene;
	Layer* layer;

	ofEvent<ObjectEventArgs> positionChanged;
	ofEvent<ObjectEventArgs> orientationChanged;
	ofEvent<ObjectEventArgs> scaleChanged;
	ofEvent<ObjectEventArgs> onHide;
	ofEvent<ObjectEventArgs> onShow;

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
