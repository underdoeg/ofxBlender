#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
	glEnable(GL_DEPTH_TEST);
	//ofSetSmoothLighting(true);

	blendFile.load("test.blend");

	glShadeModel(GL_SMOOTH);
	glEnable(GL_RESCALE_NORMAL);
	glEnable(GL_DEPTH_TEST);
}

//--------------------------------------------------------------
void testApp::update()
{

}

//--------------------------------------------------------------
void testApp::draw()
{
	cam.begin();
	ofScale(50, 50, 50);
	blendFile.getActiveScene()->activeCamera = NULL;
	blendFile.getActiveScene()->draw();
	//ofSphere(0, 0, 0, 10);
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{

}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y)
{

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{

}
