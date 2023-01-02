#ifndef _RAVEN_ACCELERATE_KD_TREE_H_
#define _RAVEN_ACCELERATE_KD_TREE_H_

#include<Raven/core/accelerate.h>
#include<Raven/core/math.h>
#include<Raven/core/ray.h>

namespace Raven {
	struct KdTreeNode;
	struct BoundEdge;
	struct IntersectInfo;
	class KdTreeAccel final :public Accelerate {
	private:
		void buildNode(int* nodeNum, int depth, const int* pIndStart, int nPrimitives,
			const Bound3f& nodeBounds, const std::vector<Bound3f>& allPrimBounds,
			std::shared_ptr<BoundEdge[]> edge[3], int* prims0, int* prims1, int badRefine);
	public:
		KdTreeAccel(const std::vector<std::shared_ptr<Primitive>>& p, int maxD, int it, int tt, double eb, int num);

		KdTreeAccel(const KdTreeAccel& tree) = delete;

		bool hit(const RayDifferential& r_in, double tMax = FLT_MAX)const;

		std::optional<SurfaceInteraction> intersect(const RayDifferential& r_in)const;

		~KdTreeAccel() {
			if (treeNodes)
				delete[] treeNodes;
		}
	private:
		int maxDepth;				//树的最大深度
		const int primsThreshold;	//在一个节点中拥有的最大图元个数
		const int isectCost;		//与一个节点相交的开销，由用户指定
		const int traversalCost;	//光线在加速结构中穿梭的开销
		const double emptyBonus;	//bonus value between 0 and 1
		int nextFreeNode = 0;
		int nAccelNode = 0;
		std::vector<int> primIndices;
		KdTreeNode* treeNodes;

		struct NodeToProcess {
			const KdTreeNode* node;
			Float tMin, tMax;
		};
	};

	enum class EdgeType { EdgeStart, EdgeEnd };

	/// <summary>
	/// KdTreeNode,为了充分利用cache，数据压缩为8B，8个Node刚好能放入64B的cache line
	/// </summary>
	struct KdTreeNode {
	public:
		union {
			float splitPos;		//if this node is interior node, record the split position along the axis
			int onePrimitive;	//if this node is leaf node and only has one primitive inside, recode the index of primitive
			int indexOffset;	//if this node is leaf node and has several primitives inside, record the offset of first index inside primitiveIndices array
		};
		union {
			int flag;			//the lower two bits indicate whether this node is leaf node or record the split axis
			int nPrims;			//if this node is leaf node, the upper 30 bits record the number of primitives within this node
			int aboveChild;		//if this node is interior node, the upper 30 bits record the index of above child of this node
		};

		void createLeaf(int n, const int* prims, std::vector<int>* primIndices) {
			//set flag in lower 2 bits and store nPrims in upper 30 bits
			flag = 3;
			nPrims |= (n << 2);

			//set primitive indices
			if (n == 0) {
				onePrimitive = 0;
			}
			else if (n == 1) {
				onePrimitive = prims[0];
			}
			else {
				indexOffset = primIndices->size();
				for (int i = 0; i < n; i++) {
					primIndices->push_back(prims[i]);
				}
			}
		}

		bool isLeaf()const {
			return (flag & 3) == 3;
		}

		void createInterior(int axis, int above, double pos) {
			splitPos = pos;
			aboveChild = above << 2;
			flag |= axis;
		}

		int getAboveChild()const {
			return aboveChild >> 2;
		}

		int getAxis()const {
			return flag & 3;
		}

		float getSplitPos()const {
			return splitPos;
		}

		int getPrimNum()const {
			if ((flag & 3) != 3)
				return -1;
			return nPrims >> 2;
		}
	};

	struct BoundEdge {
		BoundEdge(bool start, double pos, int prim) :pos(pos), prim(prim) {
			type = start ? EdgeType::EdgeStart : EdgeType::EdgeEnd;
		}
		BoundEdge() {}
		EdgeType type;
		double pos;
		int prim;

	};

	struct IntersectInfo {
		int nodeInd;
		Bound3f bound;
		IntersectInfo() {}
		IntersectInfo(int index, Bound3f bound) :nodeInd(index), bound(bound) {}
	};
}

#endif	
