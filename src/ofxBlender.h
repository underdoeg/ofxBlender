#ifndef OFXBLENDER_H
#define OFXBLENDER_H

#include "ofMain.h"
#include "ofxBlenderScene.h"
#include "ofxBlenderObject.h"
#include "ofxBlenderMesh.h"
#include "ofxBlenderCamera.h"
#include "ofxBlenderMaterial.h"
#include "ofxBlenderLight.h"


//for blend loader
#include "bMain.h"
#include "Blender.h"
#include "bBlenderFile.h"

/* AVAILABLE OBJ TYPES (NOT COMPLETE)
#define OB_EMPTY                                 0
#define OB_MESH                                  1
#define OB_CURVE                                 2
#define OB_SURF                                  3
#define OB_FONT                                  4
#define OB_MBALL                                 5
#define OB_LAMP                                  10
#define OB_CAMERA                                11
#define OB_WAVE                                  21
#define OB_LATTICE                               22
#define OB_ARMATURE                              25
*/

#define OFX_BLENDER_LIGHT 10
#define OFX_BLENDER_MESH 1
#define OFX_BLENDER_CAMERA 11
#define OFX_BLENDER_EMPTY 0

#define OFX_BLENDER_LOG_CHANNEL "ofxBlender"

class ofxBlender
{
public:
	ofxBlender();
	~ofxBlender();
	bool load(string filename);
	
	bool hasObject(string name);
	
	ofxBlenderObject* getObject(string name);
	ofxBlenderMesh* getMesh(string name);
	ofxBlenderCamera* getCamera(string name);
	ofxBlenderLight* getLight(string name);
	ofxBlenderScene* getActiveScene();
	ofxBlenderMaterial* getMaterial(string name);
	
private:
	//utils
	static string getObjectName(Blender::Object* obj);
	static ofMatrix4x4 convertMatrix(float mat[4][4]);
	static ofVec3f convertVector(float v[3]);
	static ofVec3f convertVector(short int v[3]);
	static ofFloatColor convertColor(Blender::MCol col);
	
	//parsing
	void parseScenes(bParse::bListBasePtr* sceneList);
	ofxBlenderScene* parseScene(Blender::Scene* bScene);
	
	void parseObjects(bParse::bListBasePtr* objs);
	ofxBlenderObject* parseObject(Blender::Object* obj);
	ofxBlenderMesh* parseMesh(Blender::Mesh* mesh);
	ofxBlenderCamera* parseCamera(Blender::Camera* bCam);
	ofxBlenderLight* parseLight(Blender::Lamp* bLamp);
	void parseMaterials(bParse::bListBasePtr* mats);
	ofxBlenderMaterial* parseMaterial(Blender::Material* bMat);
	
	//storage
	std::map<string, ofxBlenderObject*> objectMap;
	std::map<string, ofxBlenderMaterial*> materialMap;
	std::vector<ofxBlenderScene*> sceneList;
	std::vector<ofxBlenderCamera*> cameraList;
	std::vector<ofxBlenderMesh*> meshList;
	std::vector<ofxBlenderLight*> lightList;
	std::vector<ofxBlenderMaterial*> materialList;
	
	//other
	ofxBlenderScene* activeScene;
};

#endif // OFXBLENDER_H
