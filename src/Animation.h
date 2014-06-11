#ifndef ANIMATION_H
#define ANIMATION_H

#include "Utils.h"
#include <typeindex>
#include "Interpolation.h"

namespace ofx {

namespace blender {

class Timeline;

class Animation_ {
public:

	// little helper class to have templated default handlers for timeline events
	class DefaultHandler_ {
	public:
		virtual std::type_index getType()=0;
	};

	template<typename Type>
	class DefaultHandler: public DefaultHandler_ {
	public:
		typedef std::function<void(Type&, string, int)> Listener;
		DefaultHandler(Listener _listener) {
			listener = _listener;
		}

		void call(Type t, string address, int channel) {
			listener(t, address, channel);
		}

		std::type_index getType() {
			return type_index(typeid(Type));
		}

		Listener listener;
	};

	class DefaultHandlerContainer {
	public:
		template<typename Type>
		void call(Type value, string address, int channel) {
			std::type_index type(typeid(Type));
			if(handlers.find(type) != handlers.end()) {
				DefaultHandler<Type>* handler = static_cast<DefaultHandler<Type>*>(handlers[type]);
				handler->call(value, address, channel);
			}
		}

		template<typename Type>
		void add(std::function<void(Type&, string, int)> listener) {
			DefaultHandler<Type>* handler = new DefaultHandler<Type>(listener);
			handlers[handler->getType()] = handler;
		}

		std::map<std::type_index, DefaultHandler_*> handlers;
	};
	/////////////////////////////////////////////////////////////////////

	Animation_(string address_, int channel_) {
		loop = false;
		internalTime = 0;
		timeOffset = 0;
		timeLast = timeOffset;
		channel = channel_;
		address = address_;
		defaultHandler = NULL;
	}

	void step(unsigned long long timeNow) {
		timeNow -= timeOffset;
		onStep(timeNow, timeLast);
		timeLast = timeNow;
	};

	int channel;
	string address;
	bool loop;
	unsigned long long timeOffset;
	unsigned long long timeLast;

protected:
	class Keyframe {
	public:
		Keyframe(unsigned long long _time) {
			time = _time;
		}
		unsigned long long time;
	};
	virtual void onStep(unsigned long long timeNow, unsigned long long timeLast) = 0;
	struct keyFrameSort {
		inline bool operator() (const Keyframe* k1, const Keyframe* k2) {
			return (k1->time < k2->time);
		}
	};

	void addKeyframe(Keyframe* keyframe) {
		keyframes.push_back(keyframe);
		std::sort(keyframes.begin(), keyframes.end(), keyFrameSort());
	}

	Keyframe* getKeyframeBefore(unsigned long long time) {
		Keyframe* lastKey = NULL;
		for(Keyframe* key: keyframes) {
			if(key->time > time)
				return lastKey;
			lastKey = key;
		}
		return lastKey;
	}

	Keyframe* getKeyframeAfter(unsigned long long time) {
		for(Keyframe* key: keyframes) {
			if(key->time > time)
				return key;
		}
		return NULL;
	}

	std::vector<Keyframe*> keyframes;
	unsigned long long internalTime;
	DefaultHandlerContainer* defaultHandler;

	friend class Timeline;
};

template<typename Type>
class Animation:public Animation_ {
public:

	class Keyframe: public Animation_::Keyframe {
	public:
		Keyframe(unsigned long long _time, Type v, ofVec2f p, ofVec2f h1, ofVec2f h2):Animation_::Keyframe(_time) {
			point = p;
			handle1 = h1;
			handle2 = h2;
			interpolation = BEZIER;
			value = v;
		}

		Keyframe(unsigned long long _time, Type v, InterpolationType type):Animation_::Keyframe(_time) {
			interpolation = type;
			value = v;
		}

		ofVec2f point;
		ofVec2f handle1;
		ofVec2f handle2;
		Type value;
		InterpolationType interpolation;
	};

	Animation(string address, int channel):Animation_(address, channel) {
		oldValueSet = false;
	};

	void onStep(unsigned long long timeNow, unsigned long long timeLast) {
				
		Keyframe* key1 = static_cast<Keyframe*>(Animation_::getKeyframeBefore(timeNow));
		Keyframe* key2 = static_cast<Keyframe*>(Animation_::getKeyframeAfter(timeNow));
		
		//check if we have a key1, otherwise I don't know how to calculate this
		if(!key1)
			return;

		//if there is no key in the future, then the value has to be key1
		if(!key2 || key1->interpolation == CONSTANT) {
			Animation<Type>::triggerListeners(key1->value);
			return;
		}

		//both keys are around, let's tween
		double step = key2->time - key1->time;
		float stepRel = (timeNow - key1->time) / step;

		switch(key1->interpolation) {
			case BEZIER:
				Animation<Type>::triggerListeners(Interpolation::linear(stepRel, key1->value, key2->value));
				break;
			case LINEAR:
				Animation<Type>::triggerListeners(Interpolation::bezier(stepRel, key1->point, key1->handle2, key2->handle1, key2->point));
				break;
			case CONSTANT:
				Animation<Type>::triggerListeners(key1->value);
		}
	}

	void addKeyframe(double time, Type value, ofVec2f p, ofVec2f h1, ofVec2f h2) {
		Keyframe* key = new Keyframe(time, value, p, h1, h2);
		Animation_::addKeyframe(key);
	}

	void addKeyframe(double time, Type value, InterpolationType type = LINEAR) {
		Keyframe* key = new Keyframe(time, value, type);
		Animation_::addKeyframe(key);
	}

	typedef std::function<void(Type&, string, int)> Listener;
	void addListener(Listener listener) {
		listeners.push_back(listener);
	}

	void triggerListeners(Type value) {
		//don't do unnecessary triggers
		if(oldValueSet && oldValue == value)
			return;

		//call teh default handler
		if(defaultHandler)
			defaultHandler->call<Type>(value, address, channel);

		for(Listener listener: listeners) {
			listener(value, address, channel);
		}
		oldValue = value;
		oldValueSet = true;
	}

private:
	std::vector<Listener> listeners;
	Type oldValue;
	bool oldValueSet;
};


/////////////////////////////////////////////////// TIMELINE

class Timeline {
public:
	Timeline();
	~Timeline();

	template<typename Type>
	void setDefaultHandler(std::function<void(Type&, string, int)> listener) {
		defaultHandler.add<Type>(listener);
	}

	void add(Timeline* timeline);
	void add(Animation_* animation);

	void start();
	void step();

	void setDuration(unsigned long long duration);
	void setLoop(bool loopState);

private:
	Animation_::DefaultHandlerContainer defaultHandler;
	void setTime(unsigned long long time);
	std::vector<Animation_*> animations;
	std::vector<Timeline*> children;
	unsigned long long time;
	unsigned long long duration;
	unsigned long long timeOffset;
	bool loop;
};

}
}

#endif // ANIMATION_H
