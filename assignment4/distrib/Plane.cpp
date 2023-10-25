#include "Plane.h"

using namespace std;

bool Plane::intersect(const Ray& r, Hit& h, float tmin) {
	Vector3f ray_origin = r.getOrigin();
	Vector3f ray_direction = r.getDirection().normalized();
	float normal_dot_direction = Vector3f::dot(this->normal, ray_direction);

	if (normal_dot_direction == 0) {
		return false;
	}

	float normal_dot_origin = Vector3f::dot(this->normal, ray_origin);
	float t = -1.0 * (-1.0 * this->d + normal_dot_origin) / normal_dot_direction;

	if (t > tmin && t < h.getT()) {
		h.set(t, this->material, this->normal);
		return true;
	}
	return false;
}