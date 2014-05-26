#include "Layer.h"

namespace ofx {

namespace blender {

Layer::Layer() {
	visible = true;
	cout << "NEW LAYER" << endl;
}

Layer::~Layer() {
}

void Layer::add(Object* obj) {
	obj->layer = this;
}

void Layer::hide() {
	visible = false;
}

bool Layer::isVisible() {
	return visible;
}

void Layer::show() {
	visible = true;
}

void Layer::toggleVisibility() {
	if(isVisible())
		hide();
	else
		show();
}

}
}
