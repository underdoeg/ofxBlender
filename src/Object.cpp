#include "Object.h"

namespace ofx {

namespace blender {

Object::Object() {
	type = UNDEFINED;
	scene = NULL;
	timeline.setDefaultHandler<float>(std::bind(&Object::onAnimationData, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

Object::~Object() {
}

void Object::addChild(Object* child) {
	child->setParent(*this);
	children.push_back(child);
}

void Object::onAnimationData(float value, string address, int channel) {
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

}
}
