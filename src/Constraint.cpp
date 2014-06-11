#include "Constraint.h"

#include "Object.h"

namespace ofx {

namespace blender {

Constraint::Constraint() {
}

Constraint::~Constraint() {
}

void Constraint::setup(Object* obj) {
	object = obj;
	onSetup();
}

////////////////////////////////////////////////////////////////////////////////// TRACK TO
ofx::blender::TrackToConstraint::TrackToConstraint(Object* t, ofVec3f u) {
	target = t;
	up = u;

	ofAddListener(target->positionChanged, this, &TrackToConstraint::onTargetPositionChanged);
}

void TrackToConstraint::onPositionChanged() {
	updateLookAt();
}

void TrackToConstraint::onTargetPositionChanged(Object*& obj) {
	updateLookAt();
}

void TrackToConstraint::updateLookAt() {
	object->lookAt(target->getGlobalPosition(), up);
}

}
}
