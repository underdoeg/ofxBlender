#include "Material.h"

namespace ofx {

namespace blender {

Material::Material() {
	scene = NULL;
}

Material::~Material() {
}

void Material::begin() {
	ofEnableNormalizedTexCoords();
	if(textures.size()>0) {
		if(textures[0]->img.isAllocated()) {
			textures[0]->img.getTextureReference().bind();
		}
	}
	material.begin();
}

void Material::end() {
	material.end();
	if(textures.size()>0) {
		if(textures[0]->img.isAllocated()) {
			textures[0]->img.getTextureReference().unbind();
		}
	}
}

}
}
