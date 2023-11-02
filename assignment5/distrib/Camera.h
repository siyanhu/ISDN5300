
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
		Vector3f rayDirect = (((u * point[0] + v * point[1])) + alpha * w).normalized();
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

class OrthogonalCamera: public Camera
{
public:
	OrthogonalCamera(const Vector3f& center, const Vector3f& direction, const Vector3f& up, float imgSize) {
		centre = center;
		//cout << "Perspective camera centre: " << centre[0] << "," << centre[1] << "," << centre[2] << endl;
		w = direction.normalized();
		//cout << "Perspective camera w: " << w[0] << "," << w[1] << "," << w[2] << endl;
		u = (Vector3f::cross(w, up)).normalized();
		//cout << "Perspective camer u: " << u[0] << "," << u[1] << "," << u[2] << endl;
		v = (Vector3f::cross(u, w)).normalized();
		//cout << "Perspective camera v: " << v[0] << "," << v[1] << "," << v[2] << endl;
		img_Size = imgSize;
		//cout << "Perspective camera angle: " << angel << endl;
	}

	virtual Ray generateRay(const Vector2f& point) {
		Vector3f start = centre + u * point[0] * img_Size / 2.0f + v * point[1] * img_Size / 2.0f;
		Ray r = Ray(start, w);
		return r;
	}

	virtual float getTMin() const {
		return -FLT_MAX;
	}

private:
	Vector3f centre;
	Vector3f w;
	Vector3f u;
	Vector3f v;
	float img_Size;

};

class FisheyeCamera : public Camera {
public:
	FisheyeCamera(const Vector3f& center, const Vector3f& direction, const Vector3f& up) {
		centre = center;
		//cout << "Perspective camera centre: " << centre[0] << "," << centre[1] << "," << centre[2] << endl;
		w = direction.normalized();
		//cout << "Perspective camera w: " << w[0] << "," << w[1] << "," << w[2] << endl;
		u = (Vector3f::cross(w, up)).normalized();
		//cout << "Perspective camer u: " << u[0] << "," << u[1] << "," << u[2] << endl;
		v = (Vector3f::cross(u, w)).normalized();
		//cout << "Perspective camera v: " << v[0] << "," << v[1] << "," << v[2] << endl;
	}

	virtual Ray generateRay(const Vector2f& point) {
		float rr = sqrt(point[0] * point[0] + point[1] * point[1]);
		float phi = rr * 3.1415936 / 2.0f;
		float theta = atan2(point[1], point[0]);

		Vector3f start;
		start[0] = sin(phi) * cos(theta);
		start[1] = sin(phi) * sin(theta);
		start[2] = cos(phi);
		start.normalize();

		Vector3f rayDirect = (((u * start[0] + v * start[1])) + start[2] * w).normalized();
		Ray r = Ray(centre, rayDirect);
		return r;
	}

	virtual float getTMin() const {
		return -FLT_MAX;
	}

private:
	Vector3f centre;
	Vector3f w;
	Vector3f u;
	Vector3f v;
};

#endif //CAMERA_H
