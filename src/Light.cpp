#include "Light.h"

namespace ofx {

namespace blender {

Light::Light() {
	type = LIGHT;
	light.setParent(*this);
}

Light::~Light() {
}

void Light::begin() {
	if(scene){

	}
	light.enable();
}

void Light::end() {
	light.disable();
}

void Light::customDraw()
{
	ofDrawAxis(.5);
}

}
}

