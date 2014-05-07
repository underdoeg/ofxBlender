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
		ofVec3f scale = getPosition();
		if(address == "x"){
			scale.x = value;
		}else if(address == "y"){
			scale.y = value;
		}else if(address == "z"){
			scale.z = value;
		}
		setPosition(scale);
	}
	if(channel == "scale"){
		ofVec3f scale = getScale();
		if(address == "x"){
			scale.x = value;
		}else if(address == "y"){
			scale.y = value;
		}else if(address == "z"){
			scale.z = value;
		}
		setScale(scale);
	}
}

}
}
