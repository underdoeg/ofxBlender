#ifndef ANIMATION_H
#define ANIMATION_H

#include "Utils.h"

namespace ofx {

namespace blender {

class Animation_ {
public:


    Animation_() {
        loop = false;
        internalTime = 0;
        timeOffset = 0;
        timeLast = timeOffset;
    }

    void step(unsigned long long timeNow) {
        timeNow -= timeOffset;
        onStep(timeNow, timeLast);
        timeLast = timeNow;
    };

    string channel;
    bool loop;
    unsigned long long timeOffset;
    unsigned long long timeLast;


protected:
    class Keyframe {
    public:
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
            if(key->time > time)
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
};

template<typename Type>
class Animation:public Animation_ {
public:
    class Keyframe: public Animation_::Keyframe {

    };

    typedef std::function<void(Type&, string)> Listener;
    void addListener(Listener listener) {
        listeners.push_back(listener);
    }
private:
    std::vector<Listener> listeners;
};

template<typename Type>
class TweenAnimation: public Animation<Type> {

    class Keyframe: public Animation_::Keyframe {
    };

    void step(unsigned long long timeNow, unsigned long long timeLast) {

    }

    void addKeyframe(Type value, double time) {
        Keyframe* key = new Keyframe();
        //addKeyframe()
    }
};

class Timeline {
public:
    Timeline();
    ~Timeline();
    virtual void addAnimation(Animation_* animation);
    void start();
    void step();

private:
    std::vector<Animation_*> animations;
    unsigned long long time;
    unsigned long long duration;
    unsigned long long timeOffset;
};

}
}

#endif // ANIMATION_H
