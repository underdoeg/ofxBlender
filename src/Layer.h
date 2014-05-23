#ifndef LAYER_H
#define LAYER_H

#include "Object.h"

namespace ofx {
namespace blender {

class Layer {
public:
	Layer();
	~Layer();
	
	void add(Object* obj);
	bool isVisible();
	void show();
	void hide();
	void toggleVisibility();
private:
	bool visible;
	std::vector<Object*> objects;
};

}
}

#endif // LAYER_H
