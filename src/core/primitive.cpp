#include"primitive.h"

namespace Raven {
	bool Primitive::hit(const Ray& r_in, double tMin, double tMax)const {
		//perform ray-geometry intersection test
		if (!shape_ptr->hit(r_in, tMin, tMax)) {
			return false;
		}
		;
		return true;
	}

	std::optional<SurfaceInteraction> Primitive::intersect(const Ray& ray, double tMin, double tMax)const {
		//判断光线是否与几何体相交
		std::optional<SurfaceInteraction> hitRecord = shape_ptr->intersect(ray, tMin, tMax);

		//光线未与几何体相交
		if (hitRecord == std::nullopt) {
			return std::nullopt;
		}

		//相交并且击中光源
		if (light_ptr.get()) {
			hitRecord->hitLight = true;
			hitRecord->light = this->getAreaLight();
		}
		else {
			hitRecord->hitLight = false;
		}
		//计算材质
		if (mate_ptr.get())
			mate_ptr->computeScarttingFunctions(*hitRecord);
		return *hitRecord;
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

	std::optional<SurfaceInteraction> TransformedPrimitive::intersect(const Ray& r_in, double tMin, double tMax)const {
		if (!primToWorld || !worldToPrim || !prim)
			return std::nullopt;

		//将光线变换到Prim坐标系下并求交
		Ray transformedRay = (*worldToPrim)(r_in);
		std::optional<SurfaceInteraction> record = prim->intersect(transformedRay, tMin, tMax);

		//未相交
		if (record == std::nullopt) {
			return std::nullopt;
		}
		else {
			*record = (*primToWorld)(*record);
			return *record;
		}
	}

	Bound3f TransformedPrimitive::worldBounds()const {
		if (prim && primToWorld)
			return (*primToWorld)(prim->worldBounds());
		else
			return Bound3f();
	}
}