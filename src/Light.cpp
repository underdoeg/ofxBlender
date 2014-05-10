#include "Light.h"

namespace ofx {

namespace blender {

Light::Light() {
	type = LIGHT;
	light.setParent(*this);
}

Light::~Light() {
}

void Light::customDraw() {
	//light.enable();
}

}

}
