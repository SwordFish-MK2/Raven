#include"accelerate.h"

namespace Raven {

	bool PrimitiveList::hit(const Ray& r_in, double tMax)const {
		for (size_t i = 0; i < prims.size(); i++) {
			if (prims[i]->hit(r_in, tMax))
				return true;
		}
		return false;
	}

	std::optional<SurfaceInteraction> PrimitiveList::intersect(const Ray& r_in, double tMax)const {
		bool flag = false;
		double closest = tMax;
		SurfaceInteraction record;
		for (int i = 0; i < prims.size(); i++) {
			bool foundIntersection = prims[i]->intersect(r_in, record, closest);
			if (foundIntersection) {
				closest = record.t;
				flag = true;
			}
		}
		if (flag)
			return record;
		else
			return std::nullopt;
	}

	Bound3f PrimitiveList::worldBounds()const {
		bool flag = false;
		Bound3f box;
		for (int i = 0; i < prims.size(); i++) {
			Bound3f b = prims[i]->worldBounds();
			box = Union(box, prims[i]->worldBounds());
		}
		return box;
	}

}