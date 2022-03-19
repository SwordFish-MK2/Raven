#include"primitive.h"

namespace Raven {
	bool Primitive::hit(const Ray& r_in, double tMin, double tMax)const {
		//perform ray-geometry intersection test
		SurfaceInteraction its;
		if (!shape_ptr->intersect(r_in, its, tMin, tMax)) {
			return false;
		}
		if (light_ptr.get()) {
			its.hitLihgt = true;
			its.light = this->getAreaLight();
			return true;
		}
		else {
			its.hitLihgt = false;
		}
		if (mate_ptr.get())
			mate_ptr->computeScarttingFunctions(its);
		return true;
	}

	bool Primitive::intersect(const Ray& ray, SurfaceInteraction& its, double tMin, double tMax)const {
		//first perform ray-geometry intersection test,computing surfaceIntersection if hit, then compute bsdf
		if (!shape_ptr->intersect(ray, its, tMin, tMax)) {
			return false;
		}
		if (light_ptr.get()) {
			its.hitLihgt = true;
			its.light = this->getAreaLight();
			return true;
		}
		else {
			its.hitLihgt = false;
		}
		if (mate_ptr.get())
			mate_ptr->computeScarttingFunctions(its);
		return true;
	}

	Bound3f Primitive::worldBounds()const {
		return shape_ptr->worldBound();
	}

	bool TransformedPrimitive::hit(const Ray& r_in, double tMin, double tMax)const {
		if (!primToWorld || !worldToPrim || !prim)
			return false;
		//transform the incident ray to primitive space then perform ray intersection test
		Ray transformedRay = Inverse(*primToWorld)(r_in);
		return prim->hit(r_in, tMin, tMax);
	}

	bool TransformedPrimitive::intersect(const Ray& r_in, SurfaceInteraction& its, double tMin, double tMax)const {
		if (!primToWorld || !worldToPrim || !prim)
			return false;
		Ray transformedRay = (*worldToPrim)(r_in);
		if (prim->intersect(transformedRay, its, tMin, tMax)) {//hit
			its = (*primToWorld)(its);
			return true;
		}
		return false;
	}

	Bound3f TransformedPrimitive::worldBounds()const {
		if (prim && primToWorld)
			return (*primToWorld)(prim->worldBounds());
		else
			return Bound3f();
	}
}