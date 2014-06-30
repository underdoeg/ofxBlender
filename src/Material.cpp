#include "Material.h"

namespace ofx {

namespace blender {

Material::Material() {
	useShader = false;
	isLightningEnabled = true;
	isTwoSided = false;
}

Material::Material(string imagePath):Material() {
	textures.push_back(new Texture(imagePath));
}

Material::~Material() {
}

void Material::begin() {

	ofSetColor(255);
	if(textures.size()>0) {
		if(textures[0]->img.isAllocated()) {
			textures[0]->img.getTextureReference().setTextureMinMagFilter(GL_LINEAR, GL_LINEAR);
			textures[0]->img.getTextureReference().bind();
		}
	}

	if(!isLightningEnabled) {
		ofSetColor(material.getDiffuseColor());
		return;
	}

	if(!useShader) {
		material.begin();
	}
}

void Material::end() {
	if(textures.size()>0) {
		if(textures[0]->img.isAllocated()) {
			textures[0]->img.getTextureReference().unbind();
		}
	}

	if(!isLightningEnabled)
		return;

	if(!useShader) {
		material.end();
	}

}

}
}
