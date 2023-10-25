#ifndef LIGHT_H
#define LIGHT_H

#include <Vector3f.h>

#include "Object3D.h"

class Light
{
public:

    Light()
    {

    }

    virtual ~Light()
    {

    }

    virtual void getIllumination( const Vector3f& p, Vector3f& dir, Vector3f& col, float& distanceToLight ) const = 0;

};

class DirectionalLight : public Light
{
public:

    DirectionalLight( const Vector3f& d, const Vector3f& c )
    {
        direction = d.normalized();
        color = c;
    }

    ~DirectionalLight()
    {

    }
  ///@param p unsed in this function
  ///@param distanceToLight not well defined because it's not a point light
    virtual void getIllumination( const Vector3f& p, Vector3f& dir, Vector3f& col, float& distanceToLight ) const
    {
        // the direction to the light is the opposite of the
        // direction of the directional light source

        //cout << "dir0 " << dir[0] << "," << dir[1] << ", " << dir[2] << endl;
        dir = -1.0 * direction;
        //cout << "dir=-position " << dir[0] << "," << dir[1] << ", " << dir[2] << endl;
        col = color;
        //cout << "col " << col[0] << ", " << col[1] << ", " << col[2] << endl;
        distanceToLight = FLT_MAX;
    }

private:

    DirectionalLight(); // don't use

    Vector3f direction;
    Vector3f color;

};

class PointLight : public Light
{
public:

    PointLight( const Vector3f& p, const Vector3f& c )
    {
        position = p;
        color = c;
    }

    ~PointLight()
    {

    }

    virtual void getIllumination( const Vector3f& p, Vector3f& dir, Vector3f& col, float& distanceToLight ) const
    {
        // the direction to the light is the opposite of the
        // direction of the directional light source
        //cout << "dir0 " << dir[0] << "," << dir[1] << ", " << dir[2] << endl;
		dir = (position-p);
        //cout << "dir=position-p " << dir[0] << "," << dir[1] << ", " << dir[2] << endl;
		dir = dir/dir.abs();
        //cout << "dir = dir/dir.abs() " << dir[0] << "," << dir[1] << ", " << dir[2] << endl;
        col = color;
        //cout << "col " << col[0] << ", " << col[1] << ", " << col[2] << endl;
    }

private:

    PointLight(); // don't use
    Vector3f position;
    Vector3f color;

};

#endif // LIGHT_H
