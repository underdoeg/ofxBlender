#ifndef OFXBLENDERSCENE_H
#define OFXBLENDERSCENE_H

#include "ofxBlenderObject.h"
#include "ofxBlenderCamera.h"
#include "ofxBlenderMesh.h"
#include "ofMain.h"

class ofxBlenderScene
{
public:
	ofxBlenderScene();
	~ofxBlenderScene();
	void draw();
	void addObject(ofxBlenderObject* obj);
	
	ofxBlenderCamera* activeCamera;
	
private:
	vector<ofxBlenderObject*> objectList;
};

#endif // OFXBLENDERSCENE_H
