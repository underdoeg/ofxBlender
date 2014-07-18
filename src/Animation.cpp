#include "Animation.h"

namespace ofx {

namespace blender {

Timeline::Timeline() {
	timeOffset = ofGetElapsedTimeMillis();
	isLoop = false;
	duration = 1000000;
	isPlaying = true;
	isEndless = false;
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

	Timeline* _this = this;
	ofNotifyEvent(preFrame, _this);

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
			stop();
			ofNotifyEvent(ended, _this);
		}
	}

	for(Marker& marker: markers) {
		if(timeOld <= marker.time && time > marker.time) {
			//markerTriggered(marker.name);
			markerQueue.push_back(marker.name);
		}
	}

	//sometimes multiple markers can be called all at once if the fps is to low, the expected behaviour though is that they are triggered one after another, so we use a caching,
	// this means loss of precision but easier handling
	if(markerQueue.size() > 0) {
		markerTriggered(markerQueue.front());
		markerQueue.pop_front();
	}

	for(Animation_* animation: animations) {
		animation->step(time);
	}
	for(Timeline* child: children) {
		child->setTime(time);
	}

	ofNotifyEvent(postFrame, _this);
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
	if(!isPaused){
		timeOffset = ofGetElapsedTimeMillis();
		timeOld = 0;
	}else{
		timeOld = time;
	}
	isPlaying = true;
	isPaused = false;
	Timeline* _this = this;
	for(Timeline* child: children) {
		child->play();
		child->timeOffset = 0;
		child->time = 0;
		child->timeOld = 0;
	}
	ofNotifyEvent(started, _this);
	//setTime(timeOffset);
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
	for(Animation_* animation: animations) {
		animation->reset();
	}
	for(Timeline* child: children) {
		child->stop();
	}
	while(markerQueue.size() > 0) {
		markerTriggered(markerQueue.front());
		markerQueue.pop_front();
	}
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

struct markerSort {
	inline bool operator() (const Marker& k1, const Marker k2) {
		return (k1.time < k2.time);
	}
};

void Timeline::addMarker(float time, string name) {
	markers.push_back(Marker(time * 1000, name));
	std::sort(markers.begin(), markers.end(), markerSort());
}

std::vector<Marker> Timeline::getMarkers() {
	return markers;
}

void Timeline::clear() {
	for(Animation_* anim: animations) {
		anim->clear();
	}
	markerQueue.clear();
}

//check if animation exists
bool Timeline::hasAnimation(string key) {
	for(Animation_* anim:animations) {
		if(anim->address == key) {
			return true;
		}
	}
	return false;
}

bool Timeline::hasAnimation(string key, int channel) {
	for(Animation_* anim:animations) {
		if(anim->channel == channel && anim->address == key) {
			return true;
		}
	}
	return false;
}

}
}
