#ifndef INTERPOLATION_H
#define INTERPOLATION_H

namespace ofx {

namespace blender {

class Interpolation {
		public:

	//taken from http://devmag.org.za/2011/04/05/bzier-curves-a-tutorial/
	static float bezier(float t, ofVec2f p0, ofVec2f p1, ofVec2f p2, ofVec2f p3) {
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

	template<typename Type>
	static Type linear(float interpol, Type a, Type b) {
		return a + (b - a) * interpol;
	}

};

}

}

#endif // INTERPOLATION_H
