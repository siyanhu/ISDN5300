#include "Sphere.h"

using namespace std;

Vector3f Sphere::calculate_normal(float tmin, float discrim, float a, float b, Hit& h, const Ray& r, bool t_minus) {
	Vector3f r_direction = r.getDirection();
	float t = (-1.0 * b - sqrtf(discrim)) / (a * 2.0);
	if (t_minus == false) {
		// t is positive
		t = (-1.0 * b + sqrtf(discrim)) / (a * 2.0);
	}
	if (t <= tmin && t <= h.getT()) {
		Vector3f new_normal = (r.getOrigin() + t * r_direction - this->centre).normalized();
		h.set(t, this->material, new_normal);
		return true;
	}
	return false;
}

bool Sphere::intersect(const Ray& r, Hit& h, float tmin) {
	Vector3f r_origin = r.getOrigin() - this->centre;
	Vector3f r_direction = r.getDirection();

	//float a = r_direction.x() * r_direction.x() + r_direction.y() * r_direction.y() + r_direction.z() * r_direction.z();
	float a = r_direction.absSquared();
	float b = 2.0 * Vector3f::dot(r_direction, r_origin);
	//float c = r_origin.x() * r_direction.x() + r_direction.y() * r_direction.y() + r_direction.z() * r_direction.z() - powf(this->radius, 2.0);
	float c = r_origin.absSquared() - pow(this->radius, 2);

	float t_discrim = powf(b, 2.0) - (4.0 * a * c);

	if (t_discrim >= 0) {
		//if (calculate_normal(tmin, t_discrim, a, b, h, r, true)) {
			//return true;
		//} else if (calculate_normal(tmin, t_discrim, a, b, h, r, false)) {
			//return true;
		//}

		float t = (-1.0 * b - sqrt(t_discrim)) / (2.0 * a);
		if ((t >= tmin) && (t < h.getT())) {
			Vector3f new_normal = (r.getOrigin() + t * r_direction - this->centre).normalized();
			h.set(t, this->material, new_normal);
			return true;
		}

		t = (-1.0 * b + sqrt(t_discrim)) / (2.0 * a);
		if ((t >= tmin) && (t < h.getT())) {
			Vector3f new_normal = (r.getOrigin() + t * r_direction - this->centre).normalized();
			h.set(t, this->material, new_normal);
			return true;
		}
	}

	return false;
}