#include "ofxBlenderCamera.h"

ofxBlenderCamera::ofxBlenderCamera()
{
	camera.setParent(*this);
}

ofxBlenderCamera::~ofxBlenderCamera()
{
	
}

void ofxBlenderCamera::begin(){
	camera.begin();
}