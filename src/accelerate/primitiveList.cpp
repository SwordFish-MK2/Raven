#include"primitiveList.h"

namespace Raven {
	bool PrimitiveList::hit(const Ray& r_in, double tMin, double tMax)const {
		for (size_t i = 0; i < prims.size(); i++)
			if (prims[i]->hit(r_in, tMin, tMax))
				return true;
		return false;
	}

	bool PrimitiveList::intersect(const Ray& r_in, SurfaceInteraction& its, double tMin, double tMax)const {
		bool flag = false;
		SurfaceInteraction temp;
		double closest = tMax;
		for (int i = 0; i < prims.size(); i++) {
			if (prims[i]->intersect(r_in, temp, tMin, closest)) {
				closest = temp.t;
				flag = true;
			}
		}
		if (flag)
			its = temp;
		return flag;
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