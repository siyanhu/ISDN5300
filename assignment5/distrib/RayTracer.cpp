#include "RayTracer.h"
#include "Camera.h"
#include "Ray.h"
#include "Hit.h"
#include "Group.h"
#include "Material.h"
#include "Light.h"

#define EPSILON 0.001

//IMPLEMENT THESE FUNCTIONS
//These function definitions are mere suggestions. Change them as you like.
Vector3f mirrorDirection( const Vector3f& normal, const Vector3f& incoming)
{
    Vector3f result = normal * (2 * Vector3f::dot(normal, incoming)) - incoming;
    result.normalized();
	return result;
}

bool transmittedDirection( const Vector3f& normal, const Vector3f& incoming, 
        float index_n, float index_nt, 
        Vector3f& transmitted)
{
    float normal_r = index_n / index_nt;
    float normal_dot_direct = Vector3f::dot(normal, incoming);
    float radical = 1.0 - powf(normal_r, 2) * (1.0 - powf(normal_dot_direct, 2));

    if (radical < 0) {
        return false;
    }
    
    transmitted = normal * (normal_r * normal_dot_direct - sqrtf(radical)) - incoming * normal_r;
    transmitted.normalized();

    return true;
}

RayTracer::RayTracer(SceneParser* scene, int max_bounces, bool shadows):m_scene(scene) {
  group = scene->getGroup();
  m_maxBounces = max_bounces;
  has_shadows = shadows;
}

RayTracer::~RayTracer()
{
}

Vector3f RayTracer::traceRay(Ray& ray, float tmin, int bounces,
	float refr_index, Hit& hit) const
{

	if (group->intersect(ray, hit, tmin)) {
		Vector3f pixCol = Vector3f(0.0f, 0.0f, 0.0f);

		//Ambient color
		pixCol = pixCol + hit.getMaterial()->getDiffuseColor() * m_scene->getAmbientLight();

		//Diffuse color + SHADOWS
		for (int indL = 0; indL < m_scene->getNumLights(); indL++) {
			Light* lit = m_scene->getLight(indL);
			Vector3f litDir;
			Vector3f litCol;
			float distToLit;

			lit->getIllumination(ray.pointAtParameter(hit.getT()), litDir, litCol, distToLit);

			//cast shadows
			if (has_shadows) {
				Ray shadowRay = Ray(ray.pointAtParameter(hit.getT()), litDir);
				Hit shadowHit = Hit(distToLit, NULL, NULL);

				if (!group->intersect(shadowRay, shadowHit, EPSILON)) {
					Vector3f shadingCol = hit.getMaterial()->Shade(ray, hit, litDir, litCol);
					pixCol = pixCol + shadingCol;
				}
			}
			else {
				Vector3f shadingCol = hit.getMaterial()->Shade(ray, hit, litDir, litCol);
				pixCol = pixCol + shadingCol;
			}

		}

		if (bounces > m_maxBounces - 1) {
			return pixCol;
		}

		//Reflectance
		Vector3f mirrDir = mirrorDirection(hit.getNormal().normalized(), -1.0f * ray.getDirection());
		Ray mirrRay = Ray(ray.pointAtParameter(hit.getT()), mirrDir);
		Hit mirrHit = Hit(FLT_MAX, NULL, Vector3f(0.0f, 0.0f, 0.0f));
		Vector3f mirrColor = hit.getMaterial()->getSpecularColor() * traceRay(mirrRay, EPSILON, bounces + 1, refr_index, mirrHit);
		//pixCol = pixCol + mirrColor;


		//Refraction 
		float new_refr_index;
		Vector3f normal = hit.getNormal().normalized();
		if (Vector3f::dot(ray.getDirection(), normal) > 0) {
			new_refr_index = 1.0f;
			normal = normal * -1.0f;
		}
		else {
			new_refr_index = hit.getMaterial()->getRefractionIndex();
		}

		//check if not total reflection
		Vector3f transmitted;
		if (transmittedDirection(normal, -1.0f * ray.getDirection(), refr_index, new_refr_index, transmitted)) {

			Ray transRay = Ray(ray.pointAtParameter(hit.getT()), transmitted);
			Hit transHit = Hit();

			Vector3f transColor = hit.getMaterial()->getSpecularColor() * traceRay(transRay, EPSILON, bounces + 1, new_refr_index, transHit);

			float R_0 = pow((new_refr_index - refr_index) / (new_refr_index + refr_index), 2);
			//std::cout << R_0 << std::endl;

			float c;
			if (refr_index <= new_refr_index) {
				c = abs(Vector3f::dot(-1.0f * ray.getDirection(), normal));
			}
			else {
				c = abs(Vector3f::dot(transmitted, normal));
			}

			float R = R_0 + (1 - R_0) * pow(1 - c, 5);
			//std::cout << c << std::endl;
			//R = 0;

			pixCol = pixCol + (1 - R) * transColor + (R)*mirrColor;
		}

		//total relfection instead
		else {
			pixCol = pixCol + mirrColor;
		}

		return pixCol;
	}

	else {
		return m_scene->getBackgroundColor(ray.getDirection());
	}
}

//Vector3f RayTracer::traceRay(Ray& ray, float tmin, int bounces, float refr_index, Hit& hit) const
//{
//    if (group->intersect(ray, hit, tmin)) {
//        Vector3f pixel_color = Vector3f(0.0f, 0.0f, 0.0f);
//        pixel_color = pixel_color + hit.getMaterial()->getDiffuseColor() * m_scene->getAmbientLight();
//
//        //diffuse
//        for (int i = 0; i < m_scene->getNumLights(); i++) {
//            Light* lit = m_scene->getLight(i);
//            Vector3f light_direction;
//            Vector3f light_color;
//            float dist_to_light;
//
//            lit->getIllumination(ray.pointAtParameter(hit.getT()), light_direction, light_color, dist_to_light);
//
//            //cast shadow
//            if (has_shadows) {
//                Ray shadow_ray = Ray(ray.pointAtParameter(hit.getT()), light_direction);
//                Hit shadow_hit = Hit(dist_to_light, NULL, NULL);
//
//                if (group->intersect(shadow_ray, shadow_hit, EPSILON) == false) {
//                    Vector3f shading_color = hit.getMaterial()->Shade(ray, hit, light_direction, light_color);
//                    pixel_color = pixel_color + shading_color;
//                }
//            } else {
//                Vector3f shading_color = hit.getMaterial()->Shade(ray, hit, light_direction, light_color);
//                pixel_color = pixel_color + shading_color;
//            }
//        }
//
//        if (bounces > m_maxBounces - 1) {
//            return pixel_color;
//        }
//
//        //set refraction
//        float new_refr_index;
//        Vector3f normal = hit.getNormal();
//        normal.normalized();
//        if (Vector3f::dot(ray.getDirection(), normal) > 0) {
//            new_refr_index = 1.0f;
//            normal = normal * -1.0f;
//        }
//        else {
//            new_refr_index = hit.getMaterial()->getRefractionIndex();
//        }
//
//        // set reflection
//        Vector3f rf_direct = mirrorDirection(hit.getNormal().normalized(), -1.0f * ray.getDirection());
//        Ray rf_ray = Ray(ray.pointAtParameter(hit.getT()), rf_direct);
//        Hit rf_hit = Hit(FLT_MAX, NULL, Vector3f(0.0f, 0.0f, 0.0f));
//        Vector3f rf_color = hit.getMaterial()->getSpecularColor() * traceRay(rf_ray, EPSILON, bounces + 1, refr_index, rf_hit);
//
//        Vector3f transmitted;
//        if (transmittedDirection(normal, -1.0f * ray.getDirection(), refr_index, new_refr_index, transmitted)) {
//            Ray transmit_ray = Ray(ray.pointAtParameter(hit.getT()), transmitted);
//            Hit transmit_hit = Hit();
//
//            Vector3f transColor = hit.getMaterial()->getSpecularColor() * traceRay(transmit_ray, EPSILON, bounces + 1, new_refr_index, transmit_hit);
//            float r0 = powf((new_refr_index - refr_index) / (new_refr_index + refr_index), 2);
//            float c;
//            if (refr_index > new_refr_index) {
//                c = fabs(Vector3f::dot(transmitted, normal));
//            }
//            else {
//                c = fabs(Vector3f::dot(-1.0f * ray.getDirection(), normal));
//            }
//
//            float r = r0 + (1 - r0) * powf(1 - c, 5);
//            pixel_color = pixel_color + (1 - r) * transColor + r * rf_color;
//        }
//        else {
//            return pixel_color = pixel_color + rf_color;
//        }
//
//        return pixel_color;
//    }
//    else {
//        return m_scene->getBackgroundColor(ray.getDirection());
//    }
//
//    //hit = Hit( FLT_MAX, NULL, Vector3f( 0, 0, 0 ) );
//    return Vector3f(0,0,0);
//}
