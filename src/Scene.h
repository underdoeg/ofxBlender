#ifndef SCENE_H
#define SCENE_H

#include "Node.h"

namespace ofx
{

namespace blender
{

class Scene
{
public:
	Scene();
	~Scene();
	ofNode rootNode;
};

}

}

#endif // SCENE_H
