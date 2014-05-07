#include "Animation.h"

namespace ofx {

namespace blender {

Timeline::Timeline() {
	timeOffset = ofGetElapsedTimeMillis();
	loop = true;
	duration = 10000;
}

Timeline::~Timeline() {
}


void Timeline::add(Animation_* animation) {
	animation->defaultHandler = &defaultHandler;
	animations.push_back(animation);
}

void Timeline::step() {
	setTime(ofGetElapsedTimeMillis());
}

void Timeline::setTime(unsigned long long t) {
	if(loop)
		time = t % duration;
	else
		time = t;

	for(Animation_* animation: animations) {
		animation->step(time);
	}
	for(Timeline* child: children) {
		child->setTime(t);
	}
}

void Timeline::add(Timeline* timeline) {
	children.push_back(timeline);
}

void Timeline::start() {

}

}
}
