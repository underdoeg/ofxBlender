#include "Material.h"

namespace ofx {

namespace blender {

Material::Material() {
	scene = NULL;
	useShader = false;
}

Material::~Material() {
}

void Material::begin() {
	if(!useShader){
		ofEnableNormalizedTexCoords();
		if(textures.size()>0) {
			if(textures[0]->img.isAllocated()) {
				textures[0]->img.getTextureReference().bind();
			}
		}
		material.begin();
	}
}

void Material::end() {
	if(!useShader){
		material.end();
		if(textures.size()>0) {
			if(textures[0]->img.isAllocated()) {
				textures[0]->img.getTextureReference().unbind();
			}
		}
	}
}

}
}
