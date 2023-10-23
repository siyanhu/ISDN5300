#include "Group.h"

using namespace std;

Group::Group(int num_objects) :Object3D(NULL) {

}

bool Group::intersect(const Ray& r, Hit& h, float tmin) {
	bool is_intersected = false;
	for (int i = 0; i < this->object_list.size(); i++) {
		Object3D* obj = this->object_list[i];
		if (obj->intersect(r, h, tmin)) {
			is_intersected = true;
		}
	}
	return is_intersected;
}

void Group::addObject(int index, Object3D* obj) {
	this->object_list.push_back(obj);

}

int Group::getGroupSize() {
	return this->object_list.size();
}