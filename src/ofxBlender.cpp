#include "ofxBlender.h"


using namespace bParse;
using namespace Blender;

ofxBlender::ofxBlender()
{
	activeScene = NULL;
}

ofxBlender::~ofxBlender()
{
}

bool ofxBlender::load(string filename)
{
	bBlenderFile* bFile = new bBlenderFile(ofToDataPath(filename, true).c_str());
	bool isOk = bFile->ok();
	if(isOk){
		ofLogNotice(OFX_BLENDER_LOG_CHANNEL) << "file loaded " << filename;
	}else{
		ofLogError(OFX_BLENDER_LOG_CHANNEL) << "error loading file " << filename;
		return isOk;
	}
	
	bFile->parse(false);
	
	bMain* main = bFile->getMain();
	
	parseObjects(main->getObject());
	parseScenes(main->getScene());
	
	return isOk;
}

//SCENE PARSING
void ofxBlender::parseScenes(bParse::bListBasePtr* bScenes)
{
	for (int i=0; i<bScenes->size();i++)
	{
		ofxBlenderScene* scn = parseScene((Blender::Scene*)bScenes->at(i));
		if(scn)
			sceneList.push_back(scn);
	}
	if(sceneList.size() > 0)
		activeScene = sceneList[0];
	else
		ofLogWarning(OFX_BLENDER_LOG_CHANNEL) << "no scenes found in blend file";
}

ofxBlenderScene* ofxBlender::parseScene(Blender::Scene* bScene)
{
	ofxBlenderScene* scene = new ofxBlenderScene();
	for (Blender::Base* base = (Blender::Base*)bScene->base.first; base; base = base->next) {
			if (!base->object)
				continue;
			if(hasObject(getObjectName(base->object)))
				scene->addObject(getObject(getObjectName(base->object)));
	}
	if(bScene->camera)
		scene->activeCamera = getCamera(getObjectName(bScene->camera));
	return scene;
}

//OBJECT PARSING
void ofxBlender::parseObjects(bParse::bListBasePtr* objs)
{
	for (int i=0; i<objs->size();i++)
	{
		ofxBlenderObject* obj = parseObject((Blender::Object*)objs->at(i));
		if(obj)
			objectMap[obj->name] = obj;
	}
}


ofxBlenderObject* ofxBlender::parseObject(Blender::Object* bObj){
	ofLogNotice(OFX_BLENDER_LOG_CHANNEL) << "parsing object " << getObjectName(bObj);
	ofxBlenderObject* obj = NULL;
	
	//create an object according to type
	switch(bObj->type){
		case OFX_BLENDER_EMPTY:
			obj = new ofxBlenderObject();
			break;
		case OFX_BLENDER_MESH:
			obj = parseMesh((Blender::Mesh*)bObj->data);
			break;
		case OFX_BLENDER_CAMERA:
			obj = parseCamera((Blender::Camera*)bObj->data);
			break;
		case OFX_BLENDER_LIGHT:
			obj = parseLight((Blender::Lamp*)bObj->data);
			break;
		default:
			ofLogWarning(OFX_BLENDER_LOG_CHANNEL) << "unknown type: " << bObj->type;
		break;
	}
	
	//parse all other parameters
	if(obj != NULL){
		obj->name = getObjectName(bObj);
		obj->setTransformMatrix(convertMatrix(bObj->obmat));
		obj->type = bObj->type;
	}
	
	return obj;
}

ofxBlenderMesh* ofxBlender::parseMesh(Blender::Mesh* bMesh)
{
	ofLogNotice(OFX_BLENDER_LOG_CHANNEL) << "creating mesh";
	ofxBlenderMesh* mesh = new ofxBlenderMesh();
		
	//convert vertices
	for(int i=0;i<bMesh->totvert;i++){
		mesh->mesh.addVertex(convertVector(bMesh->mvert[i].co));
		mesh->mesh.addNormal(convertVector(bMesh->mvert[i].no).getNormalized());
	}
	
	MLoop* loops = bMesh->mloop;
	
	//this is a workaround to gt the average model smoothing. This value is used for the whole model for now. //TODO: make this work for individual tris
	int curSmoothCount  = 0;
	for(int i=0;i<bMesh->totpoly;i++){
		if(bMesh->mpoly[i].flag & 1)
			curSmoothCount++;
		else
			curSmoothCount--;
	}
	if(curSmoothCount<0)
		mesh->isSmooth = false;
	
	//convert faces
	for(int i=0;i<bMesh->totpoly;i++){
		MPoly poly = bMesh->mpoly[i];
		if (poly.totloop<3){
			ofLogWarning(OFX_BLENDER_LOG_CHANNEL) << "can't convert polygon with only 2 or less vertices";
			continue;
		}

		if (poly.totloop>4){
			ofLogWarning(OFX_BLENDER_LOG_CHANNEL) << "can't convert polygon with more than 4 vertices";
			continue;
		}

		if(poly.totloop==4){ //the poly has 4 vertices
			MLoop l1 = loops[poly.loopstart];
			MLoop l2 = loops[poly.loopstart+1];
			MLoop l3 = loops[poly.loopstart+2];
			MLoop l4 = loops[poly.loopstart+3];
			mesh->mesh.addTriangle(l3.v, l2.v, l1.v);
			mesh->mesh.addTriangle(l3.v, l4.v, l1.v);
		}else{ //poly is a triangle
			MLoop l1 = loops[poly.loopstart];
			MLoop l2 = loops[poly.loopstart+1];
			MLoop l3 = loops[poly.loopstart+2];
			mesh->mesh.addTriangle(l1.v, l2.v, l3.v);
		}
	}
	meshList.push_back(mesh);
	return mesh;
}

ofxBlenderCamera* ofxBlender::parseCamera(Blender::Camera* bCam)
{
	ofxBlenderCamera* cam = new ofxBlenderCamera();
	//cam->setupPerspective(true, bCam->clipsta, bCam->clipend)
	cameraList.push_back(cam);
	return cam;
}

ofxBlenderLight* ofxBlender::parseLight(Blender::Lamp* bLamp)
{
	ofxBlenderLight* light = new ofxBlenderLight();
	return light;
}

//UTILS
bool ofxBlender::hasObject(string name)
{
	return objectMap.find(name) != objectMap.end();
}

ofxBlenderObject* ofxBlender::getObject(string name)
{
	if(hasObject(name)){
		return objectMap[name];
	}
	ofLogError(OFX_BLENDER_LOG_CHANNEL) << "can't find object with name " << name << ". Returning NULL instead";
	return NULL;
}

//TODO: those functions are repetative and ugly as hell
ofxBlenderCamera* ofxBlender::getCamera(string name)
{
	ofxBlenderObject* obj = getObject(name);
	vector<ofxBlenderCamera*>::iterator it = cameraList.begin();
	while(it != cameraList.end()){
		obj == *it;
		return *it;
		it++;
	}
	ofLogError(OFX_BLENDER_LOG_CHANNEL) << "camera not found: " << name;
	return NULL;
}

ofxBlenderMesh* ofxBlender::getMesh(string name)
{
	ofxBlenderObject* obj = getObject(name);
	vector<ofxBlenderMesh*>::iterator it = meshList.begin();
	while(it != meshList.end()){
		obj == *it;
		return *it;
		it++;
	}
	ofLogError(OFX_BLENDER_LOG_CHANNEL) << "mesh not found: " << name;
	return NULL;
}

ofxBlenderLight* ofxBlender::getLight(string name)
{
	ofxBlenderObject* obj = getObject(name);
	vector<ofxBlenderLight*>::iterator it = lightList.begin();
	while(it != lightList.end()){
		obj == *it;
		return *it;
		it++;
	}
	ofLogError(OFX_BLENDER_LOG_CHANNEL) << "light not found: " << name;
	return NULL;
}

ofxBlenderScene* ofxBlender::getActiveScene()
{
	if(!activeScene)
		ofLogError(OFX_BLENDER_LOG_CHANNEL) << "no active scene available";
	return activeScene;
}

//STATIC UTILS
string ofxBlender::getObjectName(Blender::Object* obj)
{
	if(obj->id.name[0] == 'O')
		return obj->id.name+2;
	else
		return obj->id.name;
}

ofMatrix4x4 ofxBlender::convertMatrix(float bMat[4][4])
{
	ofMatrix4x4 mat(bMat[0][0], bMat[0][1], bMat[0][2], bMat[0][3], bMat[1][0], bMat[1][1], bMat[1][2], bMat[1][3], bMat[2][0], bMat[2][1], bMat[2][2], bMat[2][3], bMat[3][0], bMat[3][1], bMat[3][2], bMat[3][3]);
	return mat;
}

ofVec3f ofxBlender::convertVector(float v [3])
{
	return ofVec3f(v[0], v[1], v[2]);
}

ofVec3f ofxBlender::convertVector(short int v [3])
{
	return ofVec3f(v[0], v[1], v[2]);
}

ofFloatColor ofxBlender::convertColor(Blender::MCol col)
{
	return ofFloatColor(col.r, col.g, col.b, col.a);
}

