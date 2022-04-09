#include"accelerate.h"

namespace Raven {

	bool PrimitiveList::hit(const Ray& r_in, double tMax)const {
		for (size_t i = 0; i < prims.size(); i++) {
			SurfaceInteraction inter;
			if (prims[i]->hit(r_in, tMax))
				return true;
		}
		return false;
	}

	std::optional<SurfaceInteraction> PrimitiveList::intersect(const Ray& r_in, double tMax)const {
		bool flag = false;
		double closest = tMax;
		SurfaceInteraction inter;
		for (int i = 0; i < prims.size(); i++) {
			std::optional<SurfaceInteraction> record = prims[i]->intersect(r_in, closest);
			if (record) {
				inter = *record;
				closest = (*record).t;
				flag = true;
			}
		}
		if (flag)
			return inter;
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