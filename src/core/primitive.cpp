#include"primitive.h"

namespace Raven {
	bool Primitive::hit(const Ray& r_in, double tMax)const {
		//perform ray-geometry intersection test
		if (!shape_ptr->hit(r_in, tMax)) {
			return false;
		}
		return true;
	}

	bool Primitive::intersect(const Ray& ray, HitInfo& record, double tMax)const {
		//判断光线是否与几何体相交
		bool foundIntersection = shape_ptr->intersect(ray, record, tMax);
		return foundIntersection;
	}

	SurfaceInteraction Primitive::setInteractionProperty(const HitInfo& hitInfo, const RayDifferential& ray)const {

		//取得交点的几何信息
		SurfaceInteraction record = shape_ptr->getGeoInfo(hitInfo.pHit);

		record.computeDifferential(ray);
		//配置材质信息
		mate_ptr->computeScarttingFunctions(record);

		//配置光源信息
		if (light_ptr.get() != nullptr) {
			record.hitLight = true;
			record.light = this->getAreaLight();
		}
		else {
			record.hitLight = false;
		}
		record.wo = hitInfo.wo;

		return record;
	}

	Bound3f Primitive::worldBounds()const {
		return shape_ptr->worldBound();
	}

	std::shared_ptr<Primitive> Primitive::build(const std::shared_ptr<Shape>& s, const std::shared_ptr<Material>& m,
		const std::shared_ptr<Light>& l) {
		return std::make_shared<Primitive>(s, m, l);
	}

	bool TransformedPrimitive::hit(const Ray& r_in, double tMax)const {
		if (!primToWorld || !worldToPrim || !prim)
			return false;
		//transform the incident ray to primitive space then perform ray intersection test
		Ray transformedRay = Inverse(*primToWorld)(r_in);
		return prim->hit(r_in, tMax);
	}

	SurfaceInteraction TransformedPrimitive::setInteractionProperty(const HitInfo& pHit, const RayDifferential& ray)const {
		SurfaceInteraction record = prim->setInteractionProperty(pHit,ray);
		return (*primToWorld)(record);
	}

	Bound3f TransformedPrimitive::worldBounds()const {
		if (prim && primToWorld)
			return (*primToWorld)(prim->worldBounds());
		else
			return Bound3f();
	}

	std::shared_ptr<TransformedPrimitive> TransformedPrimitive::build(const Transform* ptw, const Transform* wtp,
		const std::shared_ptr<Primitive>& prim) {
		return std::make_shared<TransformedPrimitive>(ptw, wtp, prim);
	}
}