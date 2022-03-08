#ifndef _RAVEN_CORE_ACCELERATE_H_
#define _RAVEN_CORE_ACCELERATE_H_

#include"base.h"
#include"primitive.h"

namespace Raven {
	class Accelerate {
	public:
		Accelerate(const std::vector<Primitive>& prims) :prims(prims) {
			for (int i = 0; i < prims.size(); i++)
				worldBound = Union(prims[i].worldBounds(), worldBound);
		}
		virtual bool hit(const Ray& r_in, double tMin = 0.001F, double tMax = FLT_MAX)const = 0;
		virtual bool intersect(const Ray& r_in, SurfaceInteraction& its, double tMin = 0.001F, double tMax = FLT_MAX)const = 0;
		virtual Bound3f worldBounds()const {
			return worldBound;
		}
	protected:
		Bound3f worldBound;
	public:
		std::vector<Primitive> prims;
	};
}

#endif
