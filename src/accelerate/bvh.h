#ifndef _RAVEN_ACCELERATE_BVH_H_
#define _RAVEN_ACCELERATE_BVH_H_

#include"../core/accelerate.h"
#include"../core/base.h"

namespace Raven {

	struct BVHNode {
		void buildInterior(
			const std::shared_ptr<BVHNode> left,
			const std::shared_ptr<BVHNode> right) {
			box = Union(left->box, right->box);
			children[0] = left;
			children[1] = right;
		}

		void buildLeaf(const Bound3f& b, size_t offset, int n) {
			box = b;
			firstPrimOffset = offset;
			nPrims = n;
		}

		Bound3f box;
		std::shared_ptr<BVHNode> children[2];
		size_t firstPrimOffset;
		size_t nPrims;
	};

	struct SAHBucket {
		size_t nPrimitives;
		Bound3f box;
		SAHBucket() :nPrimitives(0), box() {}
	};

	struct PrimitiveInfo {
		PrimitiveInfo(const Bound3f& b, size_t index) :box(b), primitiveIndex(index),
			centroid(0.5 * box.pMin + 0.5 * box.pMax) {}
		PrimitiveInfo() {}
		Bound3f box;
		Point3f centroid;
		size_t primitiveIndex;
	};

	class BVHAccel final :public Accelerate {
	public:
		BVHAccel(const std::vector<std::shared_ptr<Primitive>>& prims, size_t maxPrim);

		virtual bool hit(const Ray& r_in, double tMax = FLT_MAX)const;

		virtual std::optional<SurfaceInteraction> intersect(const Ray& r_in, double tMax = FLT_MAX)const;
	private:
		//将所有的BVHNode储存在BVHAccel类中
		std::shared_ptr<BVHNode> root;
		size_t maxPrimInNode;

		std::shared_ptr<BVHNode> recursiveBuild(std::vector<PrimitiveInfo>& p, size_t start, size_t end,
			std::vector<std::shared_ptr<Primitive>>& ordered);
	};
}

#endif