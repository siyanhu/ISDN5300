#include "Triangle.h"
#include <iostream>

using namespace std;

bool Triangle::intersect(const Ray& r, Hit& h, float tmin) {

	Vector3f R_o = r.getOrigin();
	Vector3f R_d = r.getDirection();

	Matrix3f A(
		this->aa.x() - this->bb.x(), this->aa.x() - this->cc.x(), R_d.x(),
		this->aa.y() - this->bb.y(), this->aa.y() - this->cc.y(), R_d.y(),
		this->aa.z() - this->bb.z(), this->aa.z() - this->cc.z(), R_d.z());

	Matrix3f BetaM(
		this->aa.x() - R_o.x(), this->aa.x() - this->cc.x(), R_d.x(),
		this->aa.y() - R_o.y(), this->aa.y() - this->cc.y(), R_d.y(),
		this->aa.z() - R_o.z(), this->aa.z() - this->cc.z(), R_d.z());
	float beta = BetaM.determinant() / A.determinant();

	Matrix3f GammaM(
		this->aa.x() - this->bb.x(), this->aa.x() - R_o.x(), R_d.x(),
		this->aa.y() - this->bb.y(), this->aa.y() - R_o.y(), R_d.y(),
		this->aa.z() - this->bb.z(), this->aa.z() - R_o.z(), R_d.z());
	float gamma = GammaM.determinant() / A.determinant();

	Matrix3f tM(
		this->aa.x() - this->bb.x(), this->aa.x() - this->cc.x(), this->aa.x() - R_o.x(),
		this->aa.y() - this->bb.y(), this->aa.y() - this->cc.y(), this->aa.y() - R_o.y(),
		this->aa.z() - this->bb.z(), this->aa.z() - this->cc.z(), this->aa.z() - R_o.z());
	float t = tM.determinant() / A.determinant();

	cout << "Barycentric: " << A.determinant() << ", " << beta << ", " << gamma << ", " << t << endl;

	if (beta + gamma > 1) {
		return false;
	}
	if (beta < 0) {
		return false;
	}
	if (gamma < 0) {
		return false;
	}

	float alpha = 1.0f - beta - gamma;
	if (t > tmin && t < h.getT()) {

		Vector3f newNormal = (alpha * this->normals[0] + beta * this->normals[1] + gamma * this->normals[2]).normalized();
		h.set(t, this->material, newNormal);

		Vector2f newTexCoord = (alpha * this->texCoords[0] + beta * this->texCoords[1] + gamma * this->texCoords[2]);
		h.setTexCoord(newTexCoord);

		return true;
	}
	else {
		return false;
	}
}