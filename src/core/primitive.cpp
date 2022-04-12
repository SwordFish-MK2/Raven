#include"primitive.h"

namespace Raven {
	bool Primitive::hit(const Ray& r_in, double tMax)const {
		//perform ray-geometry intersection test
		if (!shape_ptr->hit(r_in, tMax)) {
			return false;
		}
		return true;
	}

	bool Primitive::intersect(const Ray& ray, SurfaceInteraction& record, double tMax)const {
		//判断光线是否与几何体相交
		bool foundIntersection = shape_ptr->intersect(ray, record, tMax);

		//光线未与几何体相交
		if (!foundIntersection) {
			return false;
		}

		//相交并且击中光源
		if (light_ptr.get()) {
			record.hitLight = true;
			record.light = this->getAreaLight();
		}
		else {
			record.hitLight = false;
		}

		//计算材质
		if (mate_ptr.get())
			mate_ptr->computeScarttingFunctions(record);
		return true;
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

	bool TransformedPrimitive::intersect(const Ray& r_in, SurfaceInteraction& record, double tMax)const {
		if (!primToWorld || !worldToPrim || !prim)
			return false;

		//将光线变换到Prim坐标系下并求交
		Ray transformedRay = (*worldToPrim)(r_in);
		if (!prim->intersect(transformedRay, record, tMax))
			return false;
		else {
			record = (*primToWorld)(record);
			return true;
		}
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