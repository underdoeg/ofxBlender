#include "ofApp.h"
#include "Scene.h"

void ofApp::setup() {
	ofEnableDepthTest();

	ofSetLogLevel("ofxBlender", OF_LOG_VERBOSE);

	ofx::blender::File file;
	file.load("test.blend");
	file.exportStructure();
	scene = file.getScene(0);
	//scene->setDebug(true);
	scene->timeline.play();
	//scene->timeline.setLoop(true);
	//scene->timeline.setEndless(true);
	
	//scene->getActiveCamera()->addConstraint(new LookAtConstraint(scene->getMesh(0)));
}

void ofApp::update() {
	scene->update();
}

void ofApp::draw() {
	ofBackground(scene->bgColor);
	scene->draw();
}

void ofApp::gotMessage(ofMessage msg) {
}

void ofApp::keyPressed(int key) {
}

void ofApp::keyReleased(int key) {
	if(key == 'd')
		scene->toggleDebug();
	else if(key == 'a'){
		//animate
		scene->getMesh(0)->animatePositionTo(ofVec3f(ofRandom(-5, 5), ofRandom(-5, 5), ofRandom(-5, 5)), ofRandom(1, 5));
		scene->getMesh(0)->animateRotationTo(ofVec3f(ofRandom(-5, 5), ofRandom(-5, 5), ofRandom(-5, 5)), ofRandom(1, 5));
		scene->getMesh(0)->animateScaleTo(ofVec3f(ofRandom(-5, 5), ofRandom(-5, 5), ofRandom(-5, 5)), ofRandom(1, 5));
	}else if(key == 's'){
		//animate
		scene->getMesh(0)->animateTo(scene->getMesh(1), 4);
	}
}

void ofApp::mouseDragged(int x, int y, int button) {
}

void ofApp::mouseMoved(int x, int y) {
}

void ofApp::mousePressed(int x, int y, int button) {
}

void ofApp::mouseReleased(int x, int y, int button) {
}

void ofApp::windowResized(int w, int h) {
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
}
