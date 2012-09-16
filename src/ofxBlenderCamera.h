#ifndef OFXBLENDERCAMERA_H
#define OFXBLENDERCAMERA_H

#include "ofxBlenderObject.h" // Base class: ofxBlenderObject

class ofxBlenderCamera: public ofxBlenderObject
{
public:
	ofxBlenderCamera();
	~ofxBlenderCamera();
	
	void begin();
	
	ofCamera camera;
};

#endif // OFXBLENDERCAMERA_H
