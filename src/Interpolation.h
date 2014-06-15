#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include "Utils.h"

namespace ofx {

namespace blender {

enum InterpolationType {
    LINEAR,
    BEZIER,
    CONSTANT
};

class Interpolation {
public:

	//taken from http://devmag.org.za/2011/04/05/bzier-curves-a-tutorial/
	template<typename Type>
	static Type bezier(float t, ofVec2f p0, ofVec2f p1, ofVec2f p2, ofVec2f p3) {
		ofLogWarning(OFX_BLENDER) << "unsupported type passed to bezier, works only with float";
		return Type();
	}


	template<typename Type>
	static Type linear(float interpol, Type a, Type b) {
		return a + (b - a) * interpol;
	}

//};
};

template<>
float Interpolation::bezier<float>(float t, ofVec2f p0, ofVec2f p1, ofVec2f p2, ofVec2f p3);

}
}

#endif // INTERPOLATION_H
