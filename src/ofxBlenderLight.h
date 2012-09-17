#ifndef OFXBLENDERLIGHT_H
#define OFXBLENDERLIGHT_H

#include "ofMain.h"
#include "ofxBlenderObject.h"

class ofxBlenderLight: public ofxBlenderObject, public ofLight
{
public:
	ofxBlenderLight();
	~ofxBlenderLight();
	void begin();
	void end();
	void onPositionChanged();

};

#endif // OFXBLENDERLIGHT_H
