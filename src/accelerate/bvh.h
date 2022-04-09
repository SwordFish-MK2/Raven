#ifndef _RAVEN_ACCELERATE_BVH_H_
#define _RAVEN_ACCELERATE_BVH_H_

#include"../core/accelerate.h"
#include"../core/base.h"

namespace Raven {

	struct BVHNode {
		Bound3f box;
		BVHNode* [2] children;
		int firstPrimOffset;
		int nPrims;
	};

	class BvhAccel final :public Accelerate {
	public:
		virtual bool hit(const Ray& r_in, double tMax = FLT_MAX)const = 0;

		virtual std::optional<SurfaceInteraction> intersect(const Ray& r_in, double tMax = FLT_MAX)const = 0;
	private:
		int maxPrimInNode;
	};
}

#endif