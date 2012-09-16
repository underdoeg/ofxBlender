#include "ofxBlenderScene.h"
#include "ofxBlender.h"

ofxBlenderScene::ofxBlenderScene()
{
	activeCamera = NULL;
}

ofxBlenderScene::~ofxBlenderScene()
{
}

void ofxBlenderScene::draw(){
	if(activeCamera != NULL)
		activeCamera->begin();
		
	vector<ofxBlenderObject*>::iterator it = objectList.begin();	
	while(it != objectList.end()){
		if((*it)->type != OFX_BLENDER_CAMERA)
			(*it)->begin();
		it++;
	}
	
	it = objectList.begin();
	while(it != objectList.end()){
		(*it)->draw();
		it++;
	}
	
	it = objectList.begin();
	while(it != objectList.end()){
		if((*it)->type != OFX_BLENDER_CAMERA)		
			(*it)->end();
		it++;
	}
	
	if(activeCamera != NULL)
		activeCamera->end();
}

void ofxBlenderScene::addObject(ofxBlenderObject* obj)
{
	if(obj == NULL){
		ofLogError(OFX_BLENDER_LOG_CHANNEL) << "can not add NULL object to scene";
		return;
	}
	objectList.push_back(obj);
}
