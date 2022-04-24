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
		HitInfo hitInfo;
		int index;
		//求最近的交点
		for (int i = 0; i < prims.size(); i++) {
			bool foundIntersection = prims[i]->intersect(r_in, hitInfo, closest);
			if (foundIntersection) {
				closest = hitInfo.hitTime;
				flag = true;
				index = i;
			}
		}

		//如果相交
		if (flag) {
			//生成最近交点的SurfaceInteraction
			SurfaceInteraction hitRecord = prims[index]->setInteractionProperty(hitInfo);
			return hitRecord;
		}
			
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