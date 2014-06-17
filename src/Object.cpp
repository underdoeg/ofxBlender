#include "Object.h"
#include "Layer.h"
#include "Scene.h"

namespace ofx {

namespace blender {

Object::Object() {
	type = UNDEFINED;
	scene = NULL;
	parent = NULL;
	visible = true;
	layer = NULL;
	lookAtTarget = NULL;
	lookAtUp.set(0, 1, 0);
	timeline.setDefaultHandler<float>(std::bind(&Object::onAnimationDataFloat, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	timeline.setDefaultHandler<bool>(std::bind(&Object::onAnimationDataBool, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	timeline.setDefaultHandler<ofVec3f>(std::bind(&Object::onAnimationDataVec3f, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	timeline.setDefaultHandler<ofQuaternion>(std::bind(&Object::onAnimationDataQuat, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

Object::~Object() {
}

void Object::update() {
	for(Constraint* constraint:constraints) {
		constraint->onUpdate();
	}
}

void Object::draw() {
	if(!visible)
		return;

	if(scene) {
		if(!scene->hasObject(this))
			return;
	}

	if(layer) {
		if(!layer->isVisible())
			return;
	}

	ofNode::transformGL();
	customDraw();
	for(Object* child: children) {
		child->draw();
	}
	ofNode::restoreTransformGL();
}

void Object::customDraw() {

}

void Object::addChild(Object* child) {
	child->parent = this;
	child->setParent(*this, true);
	children.push_back(child);
}

std::vector<Object*> Object::getChildren() {
	return children;
}

bool Object::hasParent() {
	return parent != NULL;
}

Object* Object::getParent() {
	return parent;
}

/////////////////////////////////////////////////////////////////////////////////

bool Object::isVisible() {
	return visible;
}

void Object::hide() {
	visible = false;
}

void Object::show() {
	visible = true;
}

void Object::toggleVisibility() {
	if(isVisible())
		hide();
	else
		show();
}

/////////////////////////////////////////////////////////////////////////////////////

void Object::addConstraint(Constraint* constraint) {
	constraint->setup(this);
	constraints.push_back(constraint);
}

////////////////////////////////////////////////////////////////////////////////////

void Object::onPositionChanged() {
	for(Constraint* constraint: constraints) {
		constraint->onPositionChanged();
	}
	Object* s = this;
	ofNotifyEvent(positionChanged, s);
}

void Object::onOrientationChanged() {
	for(Constraint* constraint: constraints) {
		constraint->onOrientationChanged();
	}
	Object* s = this;
	ofNotifyEvent(orientationChanged, s);
}

void Object::onScaleChanged() {
	for(Constraint* constraint: constraints) {
		constraint->onScaleChanged();
	}
	Object* s = this;
	ofNotifyEvent(scaleChanged, s);
}


/////////////////////////////////////////////////////////////////////////////////

void Object::onAnimationDataFloat(float value, string address, int channel) {
	//cout << channel << ":" << address << endl;
	//cout << "MY VALUES : " << value << endl;
	if(address == "location") {
		ofVec3f loc = getPosition();
		if(channel == 0) {
			loc.x = value;
		} else if(channel == 1) {
			loc.y = value;
		} else if(channel == 2) {
			loc.z = value;
		}
		setPosition(loc);
	} else if(address == "scale") {
		ofVec3f scale = getScale();
		if(channel == 0) {
			scale.x = value;
		} else if(channel == 1) {
			scale.y = value;
		} else if(channel == 2) {
			scale.z = value;
		}
		setScale(scale);
	} else if(address == "rotation_euler") {
		ofVec3f rot = getOrientationEuler();
		//ofQuaternion quat = getOrientationQuat();
		//ofVec3f axis;

		value = ofRadToDeg(value);
		if(channel == 0) {
			//tilt(value);
			rot.x = value;
			//axis = getXAxis();
		} else if(channel == 1) {
			rot.y = value;
			//pan(value);
			//axis = getYAxis();
		} else if(channel == 2) {
			rot.z = value;
			//roll(value);
			//axis = getZAxis();
		}
		//quat.set(axis.x, axis.y, axis.z, value);
		///setOrientation(quat);
		setOrientation(rot);
	}
}

void Object::onAnimationDataBool(bool value, string address, int channel) {
	if(address == "hide_render") {
		if(value) {
			hide();
		} else {
			show();
		}
	}
}

void Object::onAnimationDataVec3f(ofVec3f vec, string address, int channel) {
	if(address == "loc")
		setPosition(vec);
	else if(address == "rot")
		setOrientation(vec);
	else if(address == "scale")
		setScale(vec);
}

void Object::onAnimationDataQuat(ofQuaternion quat, string address, int channel) {
	if(address == "rot")
		setOrientation(quat);
}

//animate to
void Object::animateTo(Object* obj, float duration, InterpolationType interpolation) {
	animatePositionTo(obj->getPosition(), duration, interpolation);
	animateRotationTo(obj->getOrientationQuat(), duration, interpolation);
	animateScaleTo(obj->getScale(), duration, interpolation);

}

void Object::animatePositionTo(ofVec3f pos, float duration, InterpolationType interpolation) {
	timeline.animateTo(getPosition(), pos, duration, "loc", 0, interpolation);
}

void Object::animateRotationTo(ofVec3f rot, float duration, InterpolationType interpolation) {
	timeline.animateTo(getOrientationEuler(), rot, duration, "rot", 0, interpolation);
}

void Object::animateRotationTo(ofQuaternion rot, float duration, InterpolationType interpolation) {
	timeline.animateTo(getOrientationQuat(), rot, duration, "rot", 0, interpolation);
}

void Object::animateScaleTo(ofVec3f scale, float duration, InterpolationType interpolation) {
	timeline.animateTo(getScale(), scale, duration, "scale", 0, interpolation);
}

}
}
