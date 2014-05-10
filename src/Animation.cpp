#include "Animation.h"

namespace ofx {

namespace blender {

Timeline::Timeline() {
	timeOffset = ofGetElapsedTimeMillis();
	loop = false;
	duration = 1000000;
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
	t = t - timeOffset;
	if(loop)
		time = t % duration;
	else
		time = t;

	for(Animation_* animation: animations) {
		animation->step(time);
	}
	for(Timeline* child: children) {
		child->setTime(time);
	}
}

void Timeline::add(Timeline* timeline) {
	children.push_back(timeline);
}

void Timeline::start() {

}

void Timeline::setDuration(unsigned long long d)
{
	cout << "NEW DURATION " << d << endl;
	duration = d;
}

void Timeline::setLoop(bool loopState)
{
	loop = loopState;
}

}
}

