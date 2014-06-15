#include "Animation.h"

namespace ofx {

namespace blender {

Timeline::Timeline() {
	timeOffset = ofGetElapsedTimeMillis();
	isLoop = false;
	duration = 1000000;
	isPlaying = true;
	isEndless = true;
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
	if(isLoop && !isEndless) {
		time = t % duration;
		if(time < timeOld) {
			Timeline* _this = this;
			ofNotifyEvent(ended, _this);
			ofNotifyEvent(started, _this);
		}
	} else {
		time = t;
		if(time > duration && !isEndless) {
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
	timeline->setDuration(duration);
	timeline->setLoop(isLoop);
	timeline->setEndless(isEndless);
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
	for(Timeline* child: children) {
		child->setDuration(d);
	}
}

void Timeline::setLoop(bool loopState) {
	isLoop = loopState;
	for(Timeline* child: children) {
		child->setLoop(loopState);
	}
}

void Timeline::setEndless(bool endlessState) {
	isEndless = endlessState;
	for(Timeline* child: children) {
		child->setEndless(endlessState);
	}
}

unsigned long long Timeline::getTime() {
	return time;
}

}
}
