#include "ofxBlenderLight.h"

ofxBlenderLight::ofxBlenderLight()
{
	setup();
	setAttenuation(1.1, 0, 0);
}

ofxBlenderLight::~ofxBlenderLight()
{
}

void ofxBlenderLight::begin(){
    ofEnableLighting();	
	ofLight::enable();
}

void ofxBlenderLight::end(){
	ofLight::disable();
	ofDisableLighting();
}

void ofxBlenderLight::onPositionChanged()
{
	//has to be done because ofLight does not apply the globalPosition, so parenting won't work
	//light.setPosition(getPosition());
}
