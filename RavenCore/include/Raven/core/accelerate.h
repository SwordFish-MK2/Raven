#ifndef _RAVEN_CORE_ACCELERATE_H_
#define _RAVEN_CORE_ACCELERATE_H_

#include<Raven/core/base.h>
#include<Raven/core/primitive.h>
#include<optional>
#include<Raven/core/interaction.h>
#include<Raven/core/object.h>

namespace Raven {
	class Accelerate :public RavenObject {
	public:
		Accelerate(const std::vector<std::shared_ptr<Primitive>>& prims) :prims(prims) {
			for (int i = 0; i < prims.size(); i++)
				worldBound = Union(prims[i]->worldBounds(), worldBound);
		}

		virtual bool hit(const RayDifferential& r_in, double tMax = FLT_MAX)const = 0;

		virtual std::optional<SurfaceInteraction> intersect(const RayDifferential& r_in)const = 0;

		virtual Bound3f worldBounds()const {
			return worldBound;
		}

		std::vector<std::shared_ptr<Primitive>> prims;
	protected:
		Bound3f worldBound;

	};

	class PrimitiveList :public	Accelerate {
	public:
		PrimitiveList(const std::vector<std::shared_ptr<Primitive>>& prims) :Accelerate(prims) {}

		virtual bool hit(const RayDifferential& r_in, double tMax = FLT_MAX)const;

		virtual std::optional<SurfaceInteraction> intersect(const RayDifferential& r_in)const;

		virtual Bound3f worldBounds()const;
	};


	enum class AccelType {
		List, KdTree, BVH
	};
}

#endif
