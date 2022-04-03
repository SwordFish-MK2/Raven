#ifndef _RAVEN_ACCELERATE_PRIMITIVE_LIST_H_
#define _RAVEN_ACCELERATE_PRIMITIVE_LIST_H_

#include"../core/accelerate.h"

namespace Raven {
	class PrimitiveList :public	Accelerate {
	public:
		PrimitiveList(const std::vector<std::shared_ptr<Primitive>>& prims) :Accelerate(prims) {}

		virtual bool hit(const Ray& r_in, double tMax = FLT_MAX)const;

		virtual std::optional<SurfaceInteraction> intersect(const Ray& r_in, double tMax = FLT_MAX)const;

		virtual Bound3f worldBounds()const;
	};
}
#endif