#include "Object.h"
#include "Layer.h"
#include "Scene.h"

namespace ofx {

namespace blender {

Object::Object() {
	type = EMPTY;
	scene = NULL;
	parent = NULL;
	visible = true;
	layer = NULL;
	lookAtTarget = NULL;
	lookAtUp.set(0, 1, 0);

	animIsEuler = false;

	timeline.setDefaultHandler<float>(std::bind(&Object::onAnimationDataFloat, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	timeline.setDefaultHandler<bool>(std::bind(&Object::onAnimationDataBool, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	timeline.setDefaultHandler<ofVec3f>(std::bind(&Object::onAnimationDataVec3f, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	timeline.setDefaultHandler<ofQuaternion>(std::bind(&Object::onAnimationDataQuat, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	ofAddListener(timeline.preFrame, this, &Object::onTimelinePreFrame);
	ofAddListener(timeline.postFrame, this, &Object::onTimelinePostFrame);
}

Object::~Object() {
}

void Object::update() {
	for(Constraint* constraint:constraints) {
		constraint->onUpdate();
	}
}

void Object::draw(Scene* scn, bool drawChildren) {
	if(!visible)
		return;

	if(scn) {
		if(!scn->hasObject(this))
			return;
	}

	if(layer) {
		if(!layer->isVisible())
			return;
	}

	preDraw();

	ofNode::transformGL();
	customDraw();
	ofNode::restoreTransformGL();

	if(drawChildren) {
		for(Object* child: children) {
			if(scn && scn->isDebugEnabled())
				child->draw(scn);
			else if(child->type != CAMERA && child->type != LIGHT)
				child->draw(scn);
		}
	}

	postDraw();
}

void Object::customDraw() {
}

void Object::addChild(Object* child, bool keepGlobalTransform) {
	if(hasChild(child))
		return;
	child->parent = this;
	child->setParent(*this, keepGlobalTransform);
	children.push_back(child);
}

void Object::removeChild(Object* child) {
	if(!hasChild(child))
		return;

	child->clearParent();
	children.erase(std::remove(children.begin(), children.end(), child), children.end());
}

bool Object::hasChild(Object* obj) {
	return (std::find(children.begin(), children.end(), obj) != children.end());
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

void Object::setVisible(bool state) {
	if(state)
		show();
	else
		hide();
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

void threeaxisrot(double r11, double r12, double r21, double r31, double r32, ofVec3f& res) {
	res.x = atan2( r31, r32 );
	res.y = asin ( r21 );
	res.z = atan2( r11, r12 );
}

void Object::onTimelinePreFrame(Timeline*&) {
	animIsEuler = false;

	if(timeline.hasAnimation("rotation_euler")) {
		ofQuaternion quat = getOrientationQuat();
		double w = quat.w();
		double x = quat.x();
		double y = quat.y();
		double z = quat.z();
		double sqw = w*w;
		double sqx = x*x;
		double sqy = y*y;
		double sqz = z*z;

		if(!timeline.hasAnimation("rotation_euler", 0))
			eulerRot.x =  float(atan2(2.0 * (y*z + x*w),(-sqx - sqy + sqz + sqw)) * (180.0f/PI));
		if(!timeline.hasAnimation("rotation_euler", 1))
			eulerRot.y = float(asin(-2.0 * (x*z - y*w)) * (180.0f/PI));
		if(!timeline.hasAnimation("rotation_euler", 2))
			eulerRot.z = float(atan2(2.0 * (x*y + z*w),(sqx - sqy - sqz + sqw)) * (180.0f/PI));
	}
}

void Object::onTimelinePostFrame(Timeline*&) {
	if(animIsEuler) {
		ofQuaternion QuatAroundX = ofQuaternion( eulerRot.x, ofVec3f(1.0, 0.0, 0.0) );
		ofQuaternion QuatAroundY = ofQuaternion( eulerRot.y, ofVec3f(0.0, 1.0, 0.0) );
		ofQuaternion QuatAroundZ = ofQuaternion( eulerRot.z, ofVec3f(0.0, 0.0, 1.0) );
		setOrientation(QuatAroundX * QuatAroundY * QuatAroundZ);
	}
}

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
		value = ofRadToDeg(value);
		if(channel == 0) {
			eulerRot.x = value;
		} else if(channel == 1) {
			eulerRot.y = value;
		} else if(channel == 2) {
			eulerRot.z = value;
		}
		animIsEuler = true;
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
void Object::interpolateTo(Object* obj, float t) {
	ofVec3f globalPos = getGlobalPosition();
	setGlobalPosition(globalPos + (obj->getGlobalPosition() - globalPos) * t);

	ofQuaternion globalRot = getGlobalOrientation();
	setGlobalOrientation(globalRot + (obj->getGlobalOrientation() - globalRot) * t);

	ofVec3f globalScale = getScale();
	ofVec3f objScale = obj->getScale();
	if(objScale != globalScale)
		setScale(globalScale + (objScale - globalScale) * t);
}

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

ofVec2f Object::getPositionOnScreen(ofRectangle viewport) {
	ofVec2f ret;
	if(!scene) {
		ofLogWarning(OFX_BLENDER) << "Object::getPositionOnScreen - scene not set";
		return ret;
	}

	if(scene->isDebugEnabled()) {
		return scene->getDebugCamera()->worldToScreen(getGlobalPosition(), viewport);
	}

	Camera* cam = scene->getActiveCamera();
	if(!cam) {
		ofLogWarning(OFX_BLENDER) << "Object::getPositionOnScreen - scene not set";
		return ret;
	}

	return cam->camera.worldToScreen(getGlobalPosition(), viewport);
}

}
}
