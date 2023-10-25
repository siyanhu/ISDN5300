#include "Camera.h"

using namespace std;

PerspectiveCamera::PerspectiveCamera(const Vector3f& center, const Vector3f& direction, const Vector3f& up, float angle) {
	this->centre = center;
	this->w = direction;
	this->w.normalize();

	this->u = Vector3f::cross(w, up);
	this->u.normalize();

	this->v = Vector3f::cross(this->u, this->w);
	this->v.normalize();

	this->angle = angle;
}

Ray PerspectiveCamera::generateRay(const Vector2f& point) {
	float alpha = 1.0 / tan(this->angle / 2.0);
	Vector3f direction = (this->u * point[0] + this->v * point[1] + alpha * this->w);
	direction.normalize();
	Ray ray = Ray(this->center, direction);
	return ray;
}
