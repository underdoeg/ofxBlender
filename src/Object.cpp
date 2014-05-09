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

void Object::onAnimationData(float value, string channel, string address) {
	//cout << channel << ":" << address << endl;
	//cout << "MY VALUES : " << value << endl;
	if(channel == "location"){
		ofVec3f loc = getPosition();
		if(address == "x"){
			loc.x = value;
		}else if(address == "y"){
			loc.y = value;
		}else if(address == "z"){
			loc.z = value;
		}
		setPosition(loc);
	}else if(channel == "scale"){
		ofVec3f scale = getScale();
		if(address == "x"){
			scale.x = value;
		}else if(address == "y"){
			scale.y = value;
		}else if(address == "z"){
			scale.z = value;
		}
		setScale(scale);
	}else if(channel == "rotation_euler"){
		ofVec3f rot = getOrientationEuler();
		//ofQuaternion rot = getOrientationQuat();

		value = ofRadToDeg(value);
		if(address == "x"){
			rot.x = value;
		}else if(address == "y"){
			rot.y = value;
		}else if(address == "z"){
			rot.z = value;
		}
		setOrientation(rot);
	}
}

}
}
