#include "Animation.h"

namespace ofx {

namespace blender {

Timeline::Timeline() {
	timeOffset = ofGetElapsedTimeMillis();
	loop = false;
	duration = 1000000;
	isPlaying = true;
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
	if(!isPlaying)
		return;

	t = t - timeOffset;
	if(loop) {
		time = t % duration;
		if(time < timeOld) {
			Timeline* _this = this;
			ofNotifyEvent(ended, _this);
			ofNotifyEvent(started, _this);
		}
	} else {
		time = t;
		if(time > duration) {
			Timeline* _this = this;
			ofNotifyEvent(ended, _this);
			stop();
		}
	}


	for(Animation_* animation: animations) {
		animation->step(time);
	}
	for(Timeline* child: children) {
		child->setTime(time);
	}
	
	timeOld = time;
}

void Timeline::add(Timeline* timeline) {
	timeline->timeOffset = 0;
	children.push_back(timeline);
}

void Timeline::start() {
	isPlaying = true;
	Timeline* _this = this;
	ofNotifyEvent(started, _this);
}

void Timeline::stop() {
	isPlaying = false;
}

void Timeline::setDuration(unsigned long long d) {
	duration = d;
}

void Timeline::setLoop(bool loopState) {
	loop = loopState;
}

}
}
