#ifndef ANIMATION_H
#define ANIMATION_H

#include "Utils.h"

namespace ofx {

namespace blender {

class Animation_{
    public:
        class Keyframe{
            virtual void trigger()=0;
        };
        string channel;
};

template<typename Type>
class Animation:public Animation_{
public:
    class Keyframe: public Animation_::Keyframe{
        void trigger(){

        }
    };

    typedef std::function<void(Type&, string)> Listener;
    void addListener(Listener listener){
        listeners.push_back(listener);
    }
    private:
    std::vector<Listener> listeners;
};

template<typename Type>
class TweenAnimation: public Animation<Type>{

};

class Timeline {
public:
	Timeline();
	~Timeline();
	virtual void addAnimation(Animation_* animation);
	void step();
	std::vector<Animation_*> animations;
};

}
}

#endif // ANIMATION_H
