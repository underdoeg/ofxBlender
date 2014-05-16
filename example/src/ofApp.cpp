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

	//ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
}

void ofApp::update() {
	scene->update();
}

void ofApp::draw() {
	ofBackground(scene->bgColor);
	scene->customDraw();
}

void ofApp::gotMessage(ofMessage msg) {
}

void ofApp::keyPressed(int key) {
}

void ofApp::keyReleased(int key) {
	scene->toggleDebug();
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
