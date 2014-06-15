#include "Interpolation.h"
#include "ofMain.h"

namespace ofx {
namespace blender {

template<>
float Interpolation::bezier<float>(float t, ofVec2f p0, ofVec2f p1, ofVec2f p2, ofVec2f p3) {
	float u = 1 - t;
	float tt = t*t;
	float uu = u*u;
	float uuu = uu * u;
	float ttt = tt * t;

	ofVec2f p = uuu * p0; //first term
	p += 3 * uu * t * p1; //second term
	p += 3 * u * tt * p2; //third term
	p += ttt * p3; //fourth term

	//cout << p << endl;
	//cout << linear<float>(p.y, p0.y, p3.y) << endl;
	//return linear<Type>(linear<float>(p.y, p0.y, p3.y), a, b);
	return p.y;
}

}
}
