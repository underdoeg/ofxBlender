#include "ofApp.h"

void ofApp::setup() {
	ofSetLogLevel("ofxBlender", OF_LOG_VERBOSE);

	ofx::blender::File file;
	file.load("test.blend");
	//file.exportStructure();
}

void ofApp::update() {
}

void ofApp::draw() {
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
