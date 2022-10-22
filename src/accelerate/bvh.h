#ifndef _RAVEN_ACCELERATE_BVH_H_
#define _RAVEN_ACCELERATE_BVH_H_

#include"../core/accelerate.h"
#include"../core/base.h"

namespace Raven {

	struct BVHNode {
		void buildInterior(
			const std::shared_ptr<BVHNode> left,
			const std::shared_ptr<BVHNode> right, int axis) {
			box = Union(left->box, right->box);
			children[0] = left;
			children[1] = right;
			splitAxis = axis;
			nPrims = 0;
		}

		void buildLeaf(const Bound3f& b, size_t offset, int n) {
			box = b;
			firstPrimOffset = offset;
			nPrims = n;
		}

		int splitAxis;
		Bound3f box;

		std::shared_ptr<BVHNode> children[2];
		size_t firstPrimOffset;//�ýڵ��е�һ��ͼԪ��ͼԪ�����е�ƫ����
		size_t nPrims;//�ýڵ��к��ж��ٸ�ͼԪ
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
		Bound3f box;//ͼԪ�İ�Χ��
		Point3f centroid;//��Χ�е����ĵ�
		size_t primitiveIndex;//�������е�index
	};

	struct LinearBVHNode {
		Bound3f box;
		union {
			uint32_t firstOffset;
			uint32_t rightChild;
		};
		uint16_t nPrims;
		uint8_t axis;
		uint8_t pad;
		LinearBVHNode() :firstOffset(-1), nPrims(0), axis(0), pad(0) {

		}
	};

	class BVHAccel final :public Accelerate {
	public:
		BVHAccel(const std::vector<std::shared_ptr<Primitive>>& prims, size_t maxPrim);

		virtual bool hit(const RayDifferential& r_in, double tMax = FLT_MAX)const;

		virtual std::optional<SurfaceInteraction> intersect(const RayDifferential& r_in, double tMax = FLT_MAX)const;
	private:
		//�����е�BVHNode������BVHAccel����
		//std::shared_ptr<BVHNode> root;
		std::vector<LinearBVHNode> linearTree;
		size_t maxPrimInNode;
		int maxDepth;
		std::shared_ptr<BVHNode> recursiveBuild(std::vector<PrimitiveInfo>& p, size_t start, size_t end,
			std::vector<std::shared_ptr<Primitive>>& ordered, size_t& totalNodes, int depth = 0);

		int flattenTree(const std::shared_ptr<BVHNode>& node, int* offset);
	};
}

#endif