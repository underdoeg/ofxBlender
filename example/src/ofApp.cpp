#include "ofApp.h"
#include "Scene.h"

void ofApp::setup() {
	ofEnableDepthTest();

	ofSetLogLevel("ofxBlender", OF_LOG_VERBOSE);
	ofx::blender::File file;
	file.load("test.blend");
	file.exportStructure();

	scene = file.getScene(0);
    cout << "LOADED SCENE " << scene->name << endl;

    suzanne = file.getObject(0);
	//cout << "AMOUNT " << file.getNumberOfObjects() << endl;
	suzanne = file.getObject(0);
}

void ofApp::update() {
    scene->update();
}

void ofApp::draw() {
	easyCam.begin();
	ofDrawGrid(100);
	scene->draw();
	//suzanne->draw();
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
