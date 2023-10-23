#include "Material.h"

using namespace std;

Vector3f Material::Shade(const Ray& ray, const Hit& hit, const Vector3f& dirToLight, const Vector3f& lightColor) {
	Vector3f normal = hit.getNormal().normalized();
	Vector3f diffuse_color_temp;
	Vector3f specular_color_temp;

	float normal_dot_light = Vector3f::dot(normal, dirToLight);
	if (normal_dot_light < 0) {
		normal_dot_light = 0;
	}

	if (t.valid()) {
		Vector3f tex_color = t(hit.texCoord[0], hit.texCoord[1]);
		diffuse_color_temp = normal_dot_light * tex_color * lightColor;
		this->diffuseColor = diffuse_color_temp;
	}
	else {
		diffuse_color_temp = normal_dot_light * this->diffuseColor * lightColor;
	}

	//Phong shading
	Vector3f v = ray.getDirection() * -1.0;
	Vector3f r = (dirToLight * -1.0 + 2.0 * normal * normal_dot_light).normalized();

	float v_dot_r = Vector3f::dot(v, r);
	if (v_dot_r < 0) {
		v_dot_r = 0;
	}

	specular_color_temp = this->specularColor * powf(v_dot_r, this->shininess) * lightColor;
	return diffuse_color_temp + specular_color_temp;
}