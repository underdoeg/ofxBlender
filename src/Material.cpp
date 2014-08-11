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
	} else if(!useShader) {
		material.begin();
	}

	for(ofShader* shader: shaders) {
		if(shader->isLoaded()) {
			shader->begin();
			if(textures.size()>0) {
				if(textures[0]->img.isAllocated()) {
					shader->setUniformTexture("tex0", textures[0]->img.getTextureReference(), 0);
				}
			}
		}
	}
}

void Material::end() {
	if(textures.size()>0) {
		if(textures[0]->img.isAllocated()) {
			textures[0]->img.getTextureReference().unbind();
		}
	}

	for(ofShader* shader: shaders) {
		if(shader->isLoaded())
			shader->end();
	}

	if(!isLightningEnabled)
		return;

	if(!useShader) {
		material.end();
	}

}

bool Material::hasTransparency() {
	if(material.getDiffuseColor().a != 1.f) {
		return true;
	}
	if(material.getAmbientColor().a != 1.f) {
		return true;
	}
	if(material.getSpecularColor().a != 1.f) {
		return true;
	}

	return false;
}


void Material::addShader(ofShader* shader) {
	shaders.push_back(shader);
}

}
}
