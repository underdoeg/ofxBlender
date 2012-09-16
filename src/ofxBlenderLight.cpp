#include "ofxBlenderLight.h"

ofxBlenderLight::ofxBlenderLight()
{
	light.setParent(*this);
	light.setup();
}

ofxBlenderLight::~ofxBlenderLight()
{
}

void ofxBlenderLight::begin(){
	light.enable();
}