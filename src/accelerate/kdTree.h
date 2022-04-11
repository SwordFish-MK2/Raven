#ifndef _RAVEN_ACCELERATE_KD_TREE_H_
#define _RAVEN_ACCELERATE_KD_TREE_H_

#include"../core/accelerate.h"
#include"../core/math.h"
#include"../core/ray.h"

namespace Raven {
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

		bool isLeaf() {
			return (flag & 3) == 3;
		}

		void createInterior(int axis, int above, double pos) {
			splitPos = pos;
			aboveChild = above << 2;
			flag |= axis;
		}

		int getAboveChild() {
			return aboveChild >> 2;
		}

		int getAxis() {
			return flag & 3;
		}

		float getSplitPos() {
			return splitPos;
		}

		int getPrimNum() {
			if ((flag & 3) != 3)
				return -1;
			return nPrims >> 2;
		}
	};

	enum EdgeType {
		EdgeStart, EdgeEnd
	};

	struct BoundEdge {
		EdgeType type;
		double pos;
		int prim;

		BoundEdge(bool start, double pos, int prim) :pos(pos), prim(prim) {
			type = start ? EdgeStart : EdgeEnd;
		}

		BoundEdge() {}
	};

	struct IntersectInfo {
		int nodeInd;
		Bound3f bound;
		IntersectInfo() {}
		IntersectInfo(int index, Bound3f bound) :nodeInd(index), bound(bound) {}
	};

	class KdTreeAccel :public Accelerate {
	public:
		KdTreeAccel(const std::vector<std::shared_ptr<Primitive>>& p, int maxD, int it, int tt, double eb, int num);

		KdTreeAccel(const KdTreeAccel& tree) :Accelerate(tree.prims), maxDepth(tree.maxDepth), primsThreshold(tree.primsThreshold),
			isectCost(tree.isectCost), traversalCost(tree.traversalCost), emptyBonus(tree.emptyBonus), nAccelNode(tree.nAccelNode),
			nextFreeNode(tree.nextFreeNode), primIndices(tree.primIndices) {
			treeNodes = new KdTreeNode[nAccelNode];
			std::memcpy(treeNodes, tree.treeNodes, sizeof(KdTreeNode) * nAccelNode);
		}

		virtual bool hit(const Ray& r_in, double tMax = FLT_MAX)const;

		virtual std::optional<SurfaceInteraction> intersect(const Ray& r_in, double tMax = FLT_MAX)const;
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

		void buildNode(int* nodeNum, int depth, const int* pIndStart, int nPrimitives,
			const Bound3f& nodeBounds, const std::vector<Bound3f>& allPrimBounds,
			std::shared_ptr<BoundEdge[]> edge[3], int* prims0, int* prims1, int badRefine);
	};
}

#endif	
