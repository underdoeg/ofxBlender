#ifndef MATERIAL_H
#define MATERIAL_H

#include "Utils.h"

namespace ofx {
namespace blender {

class Texture {
public:
	Texture():isEnabled(true) {}
	string name;
	ofImage img;
	string uvLayerName;
	bool isEnabled;
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
	bool isLightningEnabled;
	bool isTwoSided;
};

}
}

#endif // MATERIAL_H
