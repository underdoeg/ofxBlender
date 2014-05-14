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

	void begin();

	ofFloatColor color;
	ofFloatColor specColor;
	std::vector<Texture> textures;
};

}}

#endif // MATERIAL_H
