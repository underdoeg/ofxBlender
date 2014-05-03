#include "ofApp.h"
#include "Scene.h"

void ofApp::setup() {
	ofEnableDepthTest();

	ofSetLogLevel("ofxBlender", OF_LOG_VERBOSE);
	ofx::blender::File file;
	file.load("test.blend");
	file.exportStructure();
	//ofx::blender::Scene* scene = file.getScene(0);
	//cout << "AMOUNT " << file.getNumberOfObjects() << endl;
	suzanne = file.getObject(0);
}

void ofApp::update() {
}

void ofApp::draw() {
	easyCam.begin();
	ofDrawGrid(100);
	suzanne->draw();
}

void ofApp::gotMessage(ofMessage msg) {
}

void ofApp::keyPressed(int key) {
}

void ofApp::keyReleased(int key) {
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
