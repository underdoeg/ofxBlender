#ifndef MATERIAL_H
#define MATERIAL_H

#include "Utils.h"

namespace ofx {
namespace blender {

class Texture {

};

class Material {
public:
	Material();
	~Material();

	std::vector<Texture> textures;
};

}}

#endif // MATERIAL_H
