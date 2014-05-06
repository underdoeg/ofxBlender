#include "Animation.h"

namespace ofx
{

namespace blender
{

Timeline::Timeline()
{
}

Timeline::~Timeline()
{
}


void Timeline::addAnimation(Animation_* animation)
{
    animations.push_back(animation);
}

void Timeline::step()
{
    long time = ofGetElapsedTimeMillis();
    for(Animation_* animation: animations){
        animation->step(time);
    }
}


}

}

