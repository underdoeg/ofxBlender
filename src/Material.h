#ifndef MATERIAL_H
#define MATERIAL_H

#include "Utils.h"

namespace ofx {
namespace blender {

class Texture {
public:
	string name;
	ofImage img;
};

class Scene;
class Material {
public:
	Material();
	~Material();

	void begin();
	void end();

	ofMaterial material;
	std::vector<Texture*> textures;
	Scene* scene;
	string name;
	bool useShader;
};

}
}

#endif // MATERIAL_H
