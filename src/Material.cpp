#include "Material.h"

namespace ofx {

namespace blender {

Material::Material() {
	color.set(1, 1, 1);
}

Material::~Material() {
}


void Material::begin() {
	ofSetColor(color);
}


}
}
