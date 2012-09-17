#ifndef OFXBLENDEROBJECT_H
#define OFXBLENDEROBJECT_H

#include "ofMain.h"

class ofxBlenderObject
{
public:
	ofxBlenderObject();
	~ofxBlenderObject();
	virtual void draw(){};
	virtual void begin(){};
	virtual void end(){};
	string name;
	int type;
};

#endif // OFXBLENDEROBJECT_H
