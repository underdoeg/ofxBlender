#include "Interpolation.h"
#include "ofMain.h"

namespace ofx {
namespace blender {

// helper for bezier
void lerp(ofVec2f& dest, const ofVec2f& a, const ofVec2f& b, const float t)
{
    dest.x = a.x + (b.x-a.x)*t;
    dest.y = a.y + (b.y-a.y)*t;
}
	
template<>
float Interpolation::bezier<float>(float t, ofVec2f a, ofVec2f b, ofVec2f c, ofVec2f d) {
	
	/*
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
	*/

	//return linear<float>(t, a.y, d.y);

	
	ofVec2f ab,bc,cd,abbc,bccd, dest;
    lerp(ab, a,b,t);           // point between a and b (green)
    lerp(bc, b,c,t);           // point between b and c (green)
    lerp(cd, c,d,t);           // point between c and d (green)
    lerp(abbc, ab,bc,t);       // point between ab and bc (blue)
    lerp(bccd, bc,cd,t);       // point between bc and cd (blue)
    lerp(dest, abbc,bccd,t);   // point on the bezier-curve (black)
	
	return dest.y;
}

}
}
