#include"kdTree.h"

namespace Raven {
	KdTreeAccel::KdTreeAccel(const std::vector<std::shared_ptr<Primitive>>& p, int maxD, int it, int tt, double eb, int primsThreshold)
		:Accelerate(p), maxDepth(maxD), isectCost(it), traversalCost(tt), emptyBonus(eb), primsThreshold(primsThreshold) {
		//compute a reasonable maxDepth if its not defined by user
		if (maxDepth <= 0)
			maxDepth = std::round(8 + 1.3 * log2l(prims.size()));

		//record bounding boxs of primitives
		std::vector<Bound3f> primBounds(p.size());

		//generate primitive indices
		std::vector<int> primNumbers(prims.size());
		for (size_t i = 0; i < p.size(); i++)
			primNumbers[i] = i;

		//allocate memory for BoundEdges
		std::shared_ptr<BoundEdge[]> edge[3];
		for (int i = 0; i < 3; i++)
			edge[i].reset(new BoundEdge[2 * prims.size()]);
		std::shared_ptr<int[]> prims0(new int[prims.size()]);
		std::shared_ptr<int[]> prims1(new int[(maxDepth + 1) * prims.size()]);
		//build root node
		std::cout << "Kd-tree start building node, primitive number: " << prims.size() << std::endl;
		int root = 0;
		buildNode(&root, maxD, &primNumbers[0], prims.size(), worldBound, primBounds,
			edge, prims0.get(), prims1.get(), 0);
		std::cout << "Kd-tree build finish, Number of tree nodes: " << nextFreeNode << std::endl;

	}

	bool KdTreeAccel::hit(const Ray& r_in, double tMax)const {
		double t0, t1;
		if (worldBound.hit(r_in, &t0, &t1)) {
			if (t0 > tMax)
				return false;
			return true;
		}
		return false;
	}

	std::optional<SurfaceInteraction> KdTreeAccel::intersect(const Ray& r_in, double tMax)const {
		SurfaceInteraction inter;
		double tMin;
		IntersectInfo nodeInfo[64];
		IntersectInfo root(0, worldBound);
		nodeInfo[0] = root;
		int head = -1;
		int rear = 0;
		int flag = false;
		Vector3f invDir(1.0 / r_in.dir[0], 1.0 / r_in.dir[1], 1.0 / r_in.dir[2]);
		bool rayNegDir[3] = { r_in.dir[0] < 0.0, r_in.dir[1] < 0.0, r_in.dir[2] < 0.0 };

		//iterate over kd-tree node to find intersection
		while (head < rear) {
			head++;//process next node
			double t0, t1;//parameter distances  =
			//test if incident ray intersect with current node
			if (nodeInfo[head].bound.hit(r_in, &t0, &t1)) {
				if (t0 > tMax || t1 < tMin)
					//miss
					continue;
				//hit 
				KdTreeNode node = treeNodes[nodeInfo[head].nodeInd];
				int i = nodeInfo[head].nodeInd;
				if (!node.isLeaf()) {
					//interior node 

					//get node split info
					int axis = node.getAxis();
					double splitPos = (double)node.getSplitPos();

					//compute intersect info of child node
					Bound3f belowBound = nodeInfo[head].bound;
					Bound3f aboveBound = nodeInfo[head].bound;
					belowBound.pMax[axis] = splitPos;
					aboveBound.pMin[axis] = splitPos;
					int belowInd = nodeInfo[head].nodeInd + 1;
					int aboveInd = node.getAboveChild();
					IntersectInfo below(belowInd, belowBound);
					IntersectInfo above(aboveInd, aboveBound);

					//decide which child node shold be processed or in what order to be processed
					double tSplit = (splitPos - r_in.origin[axis]) * invDir[axis];

					//if incident ray has negative direction in split axis, the order of processed node should be reversed
					if (rayNegDir[axis])
						std::swap(below, above);
					if (tSplit < t0)
						nodeInfo[++rear] = above;
					else if (tSplit > t1)
						nodeInfo[++rear] = below;
					else {
						nodeInfo[++rear] = below;
						nodeInfo[++rear] = above;
					}
				}
				else {
					//leaf node
					int primNum = node.getPrimNum();
					if (primNum == 1) {
						//only one primitive in this node
						std::optional<SurfaceInteraction> record = prims[node.onePrimitive]->intersect(r_in, tMax);
						if (record) {
							//incident ray hit primitive
							tMax = (*record).t;//update tMin
							inter = *record;
							flag = true;
						}
					}
					else {
						//a few primitives in this node
						for (int i = 0; i < primNum; i++) {
							int index = primIndices[node.indexOffset + i];
							std::optional<SurfaceInteraction> record = prims[index]->intersect(r_in, tMax);
							if (record) {
								//incident ray hit this primitive
								tMax = (*record).t;
								inter = *record;
								flag = true;
							}
						}
					}
				}
			}
		}
		if (flag == false)
			return std::nullopt;
		return inter;
		return std::nullopt;
	}

	void KdTreeAccel::buildNode(int* nodeNum, int depth, const int* pIndStart, int nPrimitives,
		const Bound3f& nodeBounds, const std::vector<Bound3f>& allPrimBounds,
		std::shared_ptr<BoundEdge[]> edge[3], int* prims0, int* prims1, int badRefine) {
		//if current allocated memory is used up, allocate a double sized block of memory
		if (nextFreeNode == nAccelNode) {
			//allocate double sized block of memory
			int newNodeSize = std::max(512, 2 * nAccelNode);
			KdTreeNode* newArray = new KdTreeNode[newNodeSize];
			if (nAccelNode > 0) {
				//copy original arrayy and free origianl memory 
				std::memcpy(newArray, treeNodes, sizeof(KdTreeNode) * newNodeSize);
				nAccelNode = newNodeSize;
				delete[] treeNodes;
			}
			treeNodes = newArray;
		}

		//get next freeNode
		*nodeNum = nextFreeNode++;
		KdTreeNode* node = &treeNodes[*nodeNum];
		if (nPrimitives < primsThreshold || depth >= maxDepth) {
			//create a leaf node
			node->createLeaf(nPrimitives, pIndStart, &primIndices);
			return;
		}
		else {
			//create a interior node
			int bestAxis = -1;
			int bestOffset = -1;
			double bestCost = std::numeric_limits<double>::infinity();
			int defaultAxis = nodeBounds.maxExtent();//by default,choose the axis which has the max extent
			double unSplitCost = isectCost * nPrimitives;

			int repeat = 0;
			//at most iterate 3 axis
			while (repeat < 3) {
				int axis = (defaultAxis + repeat) % 3;//switch to next axis
				//generate nodeEdge array
				int edgeNum = 0;
				for (int i = 0; i < nPrimitives; i++) {
					int primIndex = *(pIndStart + i);
					//std::shared_ptr<Primitive> p = prims[primIndex];
					Bound3f pBox = allPrimBounds[primIndex];//get bounding box of ith primitive in current node

					BoundEdge start(1, pBox.pMin[axis], primIndex);
					BoundEdge end(0, pBox.pMax[axis], primIndex);
					edge[axis][edgeNum++] = start;
					edge[axis][edgeNum++] = end;
				}

				//sort BoundEdges 
				std::sort(&edge[axis][0], &edge[axis][edgeNum],
					[](const BoundEdge& e0, const BoundEdge& e1)->bool {
						if (e0.pos == e1.pos)
							return (int)e0.type < (int)e1.type;
						else
							return e0.pos < e1.pos;
					});

				//compute cost for unsplit case
				int primsAbove = nPrimitives;
				int primsBelow = 0;
				double invArea = 1.0 / nodeBounds.surfaceArea();
				//iterate over candidate split position, compute cost
				for (int i = 0; i < edgeNum; i++) {
					if (edge[axis][i].type == EdgeEnd) primsAbove--;
					int otherAxis0 = (axis + 1) % 3;
					int otherAxis1 = (axis + 2) % 3;
					Vector3f d = nodeBounds.pMax - nodeBounds.pMin;
					double splitPos = edge[axis][i].pos;
					double aBelow = 2 * ((d[otherAxis0] * d[otherAxis1]) +
						(splitPos - nodeBounds.pMin[axis]) * d[otherAxis1] +
						(splitPos - nodeBounds.pMin[axis]) * d[otherAxis0]);
					double aAbove = 2 * ((d[otherAxis0] * d[otherAxis1]) +
						(nodeBounds.pMax[axis] - splitPos) * d[otherAxis1] +
						(nodeBounds.pMax[axis] - splitPos) * d[otherAxis0]);
					double pBelow = aBelow * invArea;
					double pAbove = aAbove * invArea;
					double eb = (primsBelow == 0 || primsAbove == 0) ? emptyBonus : 0;
					double splitCost = traversalCost +
						(1 - eb) * (pBelow * primsBelow * isectCost + pAbove * primsAbove * isectCost);
					if (splitCost < bestCost) {
						bestCost = splitCost;
						bestOffset = i;
					}
					if (edge[axis][i].type == EdgeStart)primsBelow++;
				}

				//find a good split 
				if (bestCost < unSplitCost) {
					bestAxis = axis;
					break;
				}
				//not find a good split, try another axis
				repeat++;
			}
			//not find a good split
			if (bestCost > unSplitCost)badRefine++;
			if (bestAxis == -1 || (nPrimitives < 16 && bestCost > 4 * unSplitCost) || badRefine == 3) {
				node->createLeaf(nPrimitives, pIndStart, &primIndices);
				return;
			}

			//find a good split

			//classify primitives 
			int n0 = 0, n1 = 0;
			for (int i = 0; i < bestOffset; ++i)
				if (edge[bestAxis][i].type == EdgeType::EdgeStart)
					prims0[n0++] = edge[bestAxis][i].prim;
			for (int i = bestOffset + 1; i < 2 * nPrimitives; i++)
				if (edge[bestAxis][i].type == EdgeType::EdgeEnd)
					prims1[n1++] = edge[bestAxis][i].prim;
			//compute bounds of child nodes
			Bound3f boundBelow = nodeBounds;
			Bound3f boundAbove = nodeBounds;
			boundBelow.pMax[bestAxis] = edge[bestAxis][bestOffset].pos;
			boundAbove.pMin[bestAxis] = edge[bestAxis][bestOffset].pos;
			//build child node and inite interior node for this node
			int nextBelow, nextAbove;
			buildNode(&nextBelow, depth++, prims0, n0, boundBelow, allPrimBounds, edge, prims0, prims1 + nPrimitives, badRefine);
			buildNode(&nextAbove, depth++, prims1, n1, boundAbove, allPrimBounds, edge, prims0, prims1 + nPrimitives, badRefine);
			node->createInterior(bestAxis, nextAbove, edge[bestAxis][bestOffset].pos);
			return;
		}
	}

}