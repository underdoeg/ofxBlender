#ifndef OFXBLENDEROBJECT_H
#define OFXBLENDEROBJECT_H

#include "ofMain.h"

class ofxBlenderObject: public ofNode
{
public:
	ofxBlenderObject();
	~ofxBlenderObject();
	virtual void customDraw(){};
	virtual void begin(){};
	virtual void end(){};
	
	string name;
	int type;
};

#endif // OFXBLENDEROBJECT_H
