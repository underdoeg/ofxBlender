#include "ofxBlenderMesh.h"

ofxBlenderMesh::ofxBlenderMesh()
{
	/*material = new ofMaterial();
	//material->setShininess(65);
	material->setDiffuseColor(ofColor(250, 0, 20));
	// the light highlight of the material //
	material->setSpecularColor(ofColor(250, 0, 0));
	material->setEmissiveColor(ofColor(0, 200, 0));*/
	material = NULL;
	isSmooth = true;
}

ofxBlenderMesh::~ofxBlenderMesh()
{
}

void ofxBlenderMesh::customDraw()
{
	if(isSmooth)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);		
	if(material)
		material->begin();
	mesh.draw();
	if(material)
		material->end();
}
