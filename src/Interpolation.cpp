#include "Interpolation.h"
#include "ofMain.h"

namespace ofx {
namespace blender {

// helper for bezier
void lerp(ofVec2f& dest, const ofVec2f& a, const ofVec2f& b, const float t) {
	dest.set(a.interpolated(b, t));
	/*
	dest.x = a.x + (b.x-a.x)*t;
	dest.y = a.y + (b.y-a.y)*t;
	*/
}

template<>
float Interpolation::bezier<float>(double t, ofVec2f a, ofVec2f b, ofVec2f c, ofVec2f d) {

	double u = 1 - t;
	double tt = t*t;
	double uu = u*u;
	double uuu = uu * u;
	double ttt = tt * t;

	ofVec2f p = uuu * a; //first term
	p += 3 * uu * t * b; //second term
	p += 3 * u * tt * c; //third term
	p += ttt * d; //fourth term
	
	return p.y;
	
	/*
	ofVec2f ab,bc,cd,abbc,bccd, dest;
	lerp(ab, a,b,t);           // point between a and b (green)
	lerp(bc, b,c,t);           // point between b and c (green)
	lerp(cd, c,d,t);           // point between c and d (green)
	lerp(abbc, ab,bc,t);       // point between ab and bc (blue)
	lerp(bccd, bc,cd,t);       // point between bc and cd (blue)
	lerp(dest, abbc,bccd,t);   // point on the bezier-curve (black)

	//cout << dest.y << endl;

	return dest.y;
	*/
}

}
}
