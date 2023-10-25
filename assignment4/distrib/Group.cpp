#include "Group.h"

using namespace std;

Group::Group(int num_objects) :Object3D(NULL) {
	object_list = vector<Object3D*>(num_objects);
}

bool Group::intersect(const Ray& r, Hit& h, float tmin) {
	bool is_intersected = false;
	for (int i = 0; i < this->object_list.size(); i++) {
		if (!this->object_list[i]) {
			continue;
		}
		if (this->object_list[i]->intersect(r, h, tmin)) {
			is_intersected = true;
		}
	}
	cout << is_intersected;
	return is_intersected;
}

void Group::addObject(int index, Object3D* obj) {
	this->object_list[index] = obj;
}

int Group::getGroupSize() {
	return this->object_list.size();
}