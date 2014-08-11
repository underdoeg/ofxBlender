#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "ofMain.h"

namespace ofx {

namespace blender {

class Object;
struct ObjectEventArgs;

class Constraint {
public:
	Constraint();
	~Constraint();

	void setup(Object* obj);

	virtual void onSetup() {};
	virtual void onUpdate() {};
	virtual void onOrientationChanged() {};
	virtual void onPositionChanged() {
		cout << "BLAH" << endl;
	};
	virtual void onScaleChanged() {};

	Object* object;
	string name;
};

class TrackToConstraint: public Constraint {
public:
	TrackToConstraint(Object* target, ofVec3f up = ofVec3f(0, 1, 0));

	void onPositionChanged();
	void onTargetPositionChanged(ofx::blender::ObjectEventArgs& obj);

	void updateLookAt();

	Object* target;
	ofVec3f up;
};

//some people might like this name more
typedef TrackToConstraint LookAtConstraint;

}

}

#endif // CONSTRAINT_H
