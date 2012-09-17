#include "ofxBlenderLight.h"

ofxBlenderLight::ofxBlenderLight()
{
	light.setParent(*this);
	light.setup();
	light.setAttenuation(1.1, 0, 0);
}

ofxBlenderLight::~ofxBlenderLight()
{
}

void ofxBlenderLight::begin(){
    ofEnableLighting();	
	light.enable();
}

void ofxBlenderLight::end(){
	light.disable();
	ofDisableLighting();
}

void ofxBlenderLight::onPositionChanged()
{
	//has to be done because ofLight does not apply the globalPosition, so parenting won't work
	light.setPosition(getPosition());
}
