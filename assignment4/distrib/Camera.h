
#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include <vecmath.h>
#include <float.h>
#include <cmath>


class Camera
{
public:
	//generate rays for each screen-space coordinate
	virtual Ray generateRay( const Vector2f& point ) = 0 ; 
	
	virtual float getTMin() const = 0 ; 
	virtual ~Camera(){}
protected:
	Vector3f center; 
	Vector3f direction;
	Vector3f up;
	Vector3f horizontal;

};

///TODO: Implement Perspective camera
///Fill in functions and add more fields if necessary
class PerspectiveCamera: public Camera
{
public:
	PerspectiveCamera(const Vector3f& center, const Vector3f& direction, const Vector3f& up, float angle) {
		centre = center;
		//cout << "Perspective camera centre: " << centre[0] << "," << centre[1] << "," << centre[2] << endl;
		w = direction.normalized();
		//cout << "Perspective camera w: " << w[0] << "," << w[1] << "," << w[2] << endl;
		u = (Vector3f::cross(w, up)).normalized();
		//cout << "Perspective camer u: " << u[0] << "," << u[1] << "," << u[2] << endl;
		v = (Vector3f::cross(u, w)).normalized();
		//cout << "Perspective camera v: " << v[0] << "," << v[1] << "," << v[2] << endl;
		angel = angle;
		//cout << "Perspective camera angle: " << angel << endl;

	}

	virtual Ray generateRay(const Vector2f& point) {
		//cout << "Point " << point[0] << "," << point[1] << endl;
		float alpha = 1.0f / tan(angel / 2.0f);
		Vector3f rayDirect = (((v * point[0] + u * point[1])) + alpha * w).normalized();
		//cout << " rayDirect " << rayDirect[0] << "," << rayDirect[1] << rayDirect[2] << endl;
		Ray r = Ray(centre, rayDirect);
		return r;
	}

	virtual float getTMin() const { 
		return 0.0f;
	}

private:
	Vector3f centre;
	Vector3f w, u, v;
	float angel;
};

#endif //CAMERA_H
