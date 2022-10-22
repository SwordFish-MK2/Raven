#include"accelerate.h"

namespace Raven {

	bool PrimitiveList::hit(const RayDifferential& r_in, double tMax)const {
		for (size_t i = 0; i < prims.size(); i++) {
			if (prims[i]->hit(r_in, tMax))
				return true;
		}
		return false;
	}

	std::optional<SurfaceInteraction> PrimitiveList::intersect(const RayDifferential& r_in, double tMax)const {
		bool flag = false;
		double closest = tMax;	
		HitInfo hitInfo;
		int index;

		//������Ľ���
		for (int i = 0; i < prims.size(); i++) {
			bool foundIntersection = prims[i]->intersect(r_in, hitInfo, closest);
			if (foundIntersection) {
				closest = hitInfo.hitTime;
				flag = true;
				index = i;
			}
		}

		//����ཻ
		if (flag) {
			//������������SurfaceInteraction
			SurfaceInteraction hitRecord = prims[index]->setInteractionProperty(hitInfo,r_in);
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