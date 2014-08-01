#include "Animation.h"

namespace ofx {

namespace blender {

Timeline::Timeline() {
	timeOffset = ofGetElapsedTimeMillis();
	isLoop = false;
	duration = 1000000;
	bIsPlaying = true;
	bIsEndless = false;
	bIsPaused = false;
	timeOffset = 0;
	time = 0;
	timeOld = 0;
	lastMarker = NULL;
	bPlayBackwards = false;
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
	if(!bIsPlaying)
		return;

	t = t - timeOffset;

	if(bIsPaused) {
		timeOffset += t - time;
		//time = t;
		//timeOld = time;
		return;
	}

	Timeline* _this = this;
	ofNotifyEvent(preFrame, _this);

	if(isLoop && !bIsEndless) {
		time = t % duration;
		if(time < timeOld) {
			Timeline* _this = this;
			ofNotifyEvent(ended, _this);
			ofNotifyEvent(started, _this);
			lastMarker = NULL;
		}
	} else {
		time = t;
		if(time > duration && !bIsEndless) {
			Timeline* _this = this;
			stop();
			ofNotifyEvent(ended, _this);
			return;
		}
	}

	cout << time << endl;

	//sometimes multiple markers can be called all at once if the fps is to low, the expected behaviour though is that they are triggered one after another, so we use a caching,
	// this means loss of precision but easier handling
	if(markerQueue.size() > 0) {
		triggerMarker(markerQueue.front());
		markerQueue.erase(markerQueue.begin());
	}

	for(Animation_* animation: animations) {
		animation->step(time);
	}
	for(Timeline* child: children) {
		child->setTime(time);
	}

	for(Marker* marker: markers) {
		if(timeOld <= marker->time && time > marker->time && std::find(markerQueue.begin(), markerQueue.end(), marker) == markerQueue.end()) {
			triggerMarker(marker);
			//markerTriggered(marker.name);
			//markerQueue.push_back(marker);
		}
	}

	ofNotifyEvent(postFrame, _this);
	timeOld = time;
}

void Timeline::add(Timeline* timeline) {
	timeline->timeOffset = 0;
	timeline->setDuration(duration);
	timeline->setLoop(isLoop);
	timeline->setEndless(bIsEndless);
	children.push_back(timeline);
}

void ofx::blender::Timeline::playBackwards() {
	play();
	bPlayBackwards = true;
	ofLogWarning(OFX_BLENDER) << "play backwards not yet implemented";
}

void Timeline::play() {
	if(!bIsPaused) {
		timeOffset = ofGetElapsedTimeMillis();
		timeOld = 0;
	} else {
		timeOld = ofGetElapsedTimeMillis() - timeOffset;
	}
	bIsPlaying = true;
	bIsPaused = false;
	Timeline* _this = this;
	for(Timeline* child: children) {
		child->play();
		child->timeOffset = 0;
		child->time = 0;
		child->timeOld = 0;
	}
	ofNotifyEvent(started, _this);
	//setTime(timeOffset);
	bPlayBackwards = false;
	step();
}

void Timeline::pause() {
	while(markerQueue.size() > 0) {
		triggerMarker(markerQueue.front());
		markerQueue.erase(markerQueue.begin());
	}
	bIsPaused = true;
}

void Timeline::replay() {
	stop();
	play();
}

void Timeline::stop() {
	lastMarker = NULL;
	bIsPlaying = false;
	timeOld = 0;
	//timeOffset = ofGetElapsedTimeMillis();
	for(Animation_* animation: animations) {
		animation->reset();
	}
	for(Timeline* child: children) {
		child->stop();
	}

	//markerQueue.clear();

	while(markerQueue.size() > 0) {
		triggerMarker(markerQueue.front());
		markerQueue.erase(markerQueue.begin());
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
	bIsEndless = endlessState;
	for(Timeline* child: children) {
		child->setEndless(endlessState);
	}
}

void Timeline::jumpToTime(unsigned long long t) {
	bool pauseAfter = isPaused();
	
	if(pauseAfter)
		play();
	
	timeOffset -= t - time;
	step();
	
	if(pauseAfter)
		pause();
}

unsigned long long Timeline::getTime() {
	return time;
}

unsigned long long Timeline::getDuration() {
	return duration;
}

bool Timeline::isAnimating() {
	if(bIsPaused)
		return false;
	if(!bIsPlaying)
		return false;
	for(Animation_* anim: animations) {
		if(anim->isRunning())
			return true;
	}
	return false;
}

bool ofx::blender::Timeline::isPaused() {
	return bIsPaused;
}

bool ofx::blender::Timeline::isPlaying() {
	return bIsPlaying && !bIsPaused;
}

struct markerSort {
	inline bool operator() (const Marker* k1, const Marker* k2) {
		return (k1->time < k2->time);
	}
};

void Timeline::addMarker(float time, string name) {
	markers.push_back(new Marker(time * 1000, name));
	std::sort(markers.begin(), markers.end(), markerSort());
}

std::vector<Marker*> Timeline::getMarkers() {
	return markers;
}

Marker* ofx::blender::Timeline::getNextMarker() {
	for(Marker* marker: markers) {
		if(marker->time > time) {
			return marker;
		}
	}
	return NULL;
}

Marker* ofx::blender::Timeline::getPrevMarker() {
	Marker* lastMarker = NULL;
	for(Marker* marker: markers) {
		if(marker->time > time) {
			return lastMarker;
		}
		lastMarker = marker;
	}
	return lastMarker;
}

Marker* Timeline::getMarkerBefore(Marker* pMarker) {
	Marker* lastMarker = NULL;
	for(Marker* marker: markers) {
		if(marker == pMarker) {
			return lastMarker;
		}
		lastMarker = marker;
	}
	return lastMarker;
}

void Timeline::jumpToMarker(Marker* marker) {
	jumpToTime(marker->time);
}

void Timeline::jumpToMarker(std::string markerName) {
	for(Marker* marker: markers) {
		jumpToMarker(marker);
	}
}

void Timeline::jumpToNextMarker() {
	Marker* marker = getNextMarker();
	if(marker) {
		jumpToMarker(marker);
	}
}

void Timeline::jumpToPrevMarker() {
	Marker* marker = getPrevMarker();
	if(marker) {
		jumpToMarker(marker);
	}
}

void Timeline::triggerMarker(Marker* marker) {
	if(lastMarker == marker)
		return;
	lastMarker = marker;
	markerTriggered(marker->name);
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
