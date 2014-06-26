#include "Animation.h"

namespace ofx {

namespace blender {

Timeline::Timeline() {
	timeOffset = ofGetElapsedTimeMillis();
	isLoop = false;
	duration = 1000000;
	isPlaying = true;
	isEndless = true;
	isPaused = false;
	timeOffset = 0;
	time = 0;
	timeOld = 0;
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
		
	if(isPaused) {
		timeOffset += t - time;
		//time = t;
		//timeOld = time;
		return;
	}
	
	cout << t << endl;
	

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

	for(Marker& marker: markers) {
		if(timeOld < marker.time && time > marker.time) {
			markerTriggered(marker.name);
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

void Timeline::play() {
	if(!isPaused)
		timeOffset = ofGetElapsedTimeMillis();
	
	timeOld = time;
	isPlaying = true;
	isPaused = false;
	Timeline* _this = this;
	for(Timeline* child: children) {
		child->timeOffset = 0;
		child->time = 0;
	}
	ofNotifyEvent(started, _this);
	step();
}

void Timeline::pause() {
	isPaused = true;
}

void Timeline::replay() {
	stop();
	play();
}

void Timeline::stop() {
	isPlaying = false;
	timeOld = 0;
	timeOffset = ofGetElapsedTimeMillis();
}

void Timeline::setDuration(double d) {
	duration = d * 1000;
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

unsigned long long Timeline::getDuration() {
	return duration;
}

bool Timeline::isAnimating() {
	for(Animation_* anim: animations) {
		if(anim->isRunning())
			return true;
	}
	return false;
}

void Timeline::addMarker(float time, string name) {
	markers.push_back(Marker(time * 1000, name));
}

std::vector<Marker> Timeline::getMarkers() {
	return markers;
}

}
}
