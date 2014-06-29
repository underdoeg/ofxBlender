#ifndef MATERIAL_H
#define MATERIAL_H

#include "Utils.h"
#include "Light.h"

namespace ofx {
namespace blender {

class Texture {
public:
	Texture():isEnabled(true) {}
	Texture(string imgPath):isEnabled(true) {img.loadImage(imgPath);}
	string name;
	ofImage img;
	string uvLayerName;
	bool isEnabled;
};

class Scene;
class Material {
public:
	Material();
	Material(string imagePath);
	~Material();

	void begin();
	void end();

	ofMaterial material;
	std::vector<Texture*> textures;
	string name;
	bool useShader;

private:
	friend class Scene;
	std::vector<Light*> lights;
	bool isLightningEnabled;
	bool isTwoSided;
};

}
}

#endif // MATERIAL_H
