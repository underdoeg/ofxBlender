#ifndef MATERIAL_H
#define MATERIAL_H

#include "Utils.h"

namespace ofx {
namespace blender {

class Texture {
};

class Scene;
class Material {
public:
	Material();
	~Material();

	void begin();
	void end();

	ofMaterial material;
	std::vector<Texture> textures;
	Scene* scene;
};

}
}

#endif // MATERIAL_H
