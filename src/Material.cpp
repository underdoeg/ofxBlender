#include "Material.h"

namespace ofx {

namespace blender {

Material::Material() {
	scene = NULL;
}

Material::~Material() {
}

void Material::begin() {
	material.begin();
}

void Material::end() {
	material.end();
}

}
}
