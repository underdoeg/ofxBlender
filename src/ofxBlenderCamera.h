#ifndef OFXBLENDERCAMERA_H
#define OFXBLENDERCAMERA_H

#include "ofxBlenderObject.h" // Base class: ofxBlenderObject

class ofxBlenderCamera: public ofxBlenderObject, public ofCamera
{
public:
	ofxBlenderCamera();
	~ofxBlenderCamera();
	
	void begin();
	void end();
};

#endif // OFXBLENDERCAMERA_H
