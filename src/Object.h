#ifndef BLENDER_OBJECT_H
#define BLENDER_OBJECT_H

#include "Utils.h"
#include "ofMain.h"
#include "Animation.h"

namespace ofx {
namespace blender {

enum ObjectType{
    UNDEFINED,
    MESH,
    LIGHT
};

class Object: public ofNode {
public:
    Object();
    ~Object();
    string name;
    ObjectType type;
	Animation animation;
};

}
}

#endif // OBJECT_H
