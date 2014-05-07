#ifndef ANIMATION_H
#define ANIMATION_H

#include "Utils.h"
#include <typeindex>

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
		typedef std::function<void(Type&, string, string)> Listener;
		DefaultHandler(Listener _listener) {

			listener = _listener;
		}

		void call(Type t, string channel, string address){
			listener(t, channel, address);
		}

		std::type_index getType(){
			return type_index(typeid(Type));
		}

		Listener listener;
	};

	class DefaultHandlerContainer {
	public:
		template<typename Type>
		void call(Type value, string channel, string address) {
			std::type_index type(typeid(Type));
			if(handlers.find(type) != handlers.end()) {
				DefaultHandler<Type>* handler = static_cast<DefaultHandler<Type>*>(handlers[type]);
				handler->call(value, channel, address);
			}
		}

		template<typename Type>
		void add(std::function<void(Type&, string, string)> listener) {
			DefaultHandler<Type>* handler = new DefaultHandler<Type>(listener);
			handlers[handler->getType()] = handler;
		}

		std::map<std::type_index, DefaultHandler_*> handlers;
	};
	/////////////////////////////////////////////////////////////////////

	Animation_(string channel_, string address_) {
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

	string channel;
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
		Keyframe* lastFrame = NULL;
		for(Keyframe* key: keyframes) {
			if(key->time < time)
				return key;
			lastFrame = key;
		}
		return lastFrame;
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
		Keyframe(unsigned long long _time, Type v):Animation_::Keyframe(_time) {
			value = v;
		}
		Type value;
	};

	Animation(string channel, string address):Animation_(channel, address) {
		oldValueSet = false;
	};

	typedef std::function<void(Type&, string, string)> Listener;
	void addListener(Listener listener) {
		listeners.push_back(listener);
	}

	void triggerListeners(Type value) {
		//don't do unnecessary triggers
		if(oldValueSet && oldValue == value)
			return;

		//call teh default handler
		if(defaultHandler)
			defaultHandler->call<Type>(value, channel, address);

		for(Listener listener: listeners) {
			listener(value, channel, address);
		}
		oldValue = value;
		oldValueSet = true;
	}


private:
	std::vector<Listener> listeners;
	Type oldValue;
	bool oldValueSet;
};

//TODO: implement
template<typename Type>
class TweenAnimation: public Animation<Type> {
public:
	enum TweenType {
	    LINEAR,
	    BEZIER
	};

	class Keyframe: public Animation<Type>::Keyframe {
	public:
		Keyframe(unsigned long long _time, Type v, TweenType _tweenType):Animation<Type>::Keyframe(_time, v) {
			tweenType = _tweenType;
		}
		TweenType tweenType;
	};

	TweenAnimation(string channel, string address_=""):Animation<Type>(channel, address_) {
	};

	void onStep(unsigned long long timeNow, unsigned long long timeLast) {

		Keyframe* key1 = static_cast<Keyframe*>(Animation_::getKeyframeBefore(timeNow));
		Keyframe* key2 = static_cast<Keyframe*>(Animation_::getKeyframeAfter(timeNow));

		//check if we have a key1, otherwise I don't know how to calculate this
		if(!key1)
			return;

		//if there is no key in the future, then the value has to be key1
		if(!key2) {
			Animation<Type>::triggerListeners(key1->value);
			return;
		}

		//both keys are around, let's tween
		double step = key2->time - key1->time;
		float stepRel = (timeNow - key1->time) / step;

		//TODO: not just linear please
		Animation<Type>::triggerListeners(key1->value + (key2->value - key1->value) * stepRel);
	}

	void addKeyframe(double time, Type value, TweenType tweenType=LINEAR) {
		Keyframe* key = new Keyframe(time, value, tweenType);
		//key->
		Animation_::addKeyframe(key);
	}
};


/////////////////////////////////////////////////// TIMELINE

class Timeline {
public:
	Timeline();
	~Timeline();

	template<typename Type>
	void setDefaultHandler(std::function<void(Type&, string, string)> listener) {
		defaultHandler.add<Type>(listener);
	}

	void add(Timeline* timeline);
	void add(Animation_* animation);

	void start();
	void step();

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
