#ifndef OFXBLENDERMESH_H
#define OFXBLENDERMESH_H

#include "ofxBlenderObject.h" // Base class: ofxBlenderObject

class ofxBlenderMesh : public ofxBlenderObject
{
public:
	ofxBlenderMesh();
	~ofxBlenderMesh();
	void customDraw();
	ofMesh  mesh;	
	ofMaterial* material;
	bool isSmooth;
};

#endif // OFXBLENDERMESH_H
