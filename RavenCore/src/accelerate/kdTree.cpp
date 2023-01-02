#include<Raven/accelerate/kdTree.h>

namespace Raven {
	KdTreeAccel::KdTreeAccel(
		const std::vector<std::shared_ptr<Primitive>>& p,
		int maxD,
		int it,
		int tt,
		double eb,
		int primsThreshold
	)
		:Accelerate(p),
		maxDepth(maxD),
		isectCost(it),
		traversalCost(tt),
		emptyBonus(eb),
		primsThreshold(primsThreshold)
	{
		//compute a reasonable maxDepth if its not defined by user
		if (maxDepth <= 0)
			maxDepth = std::round(8 + 1.3 * log2l(prims.size()));

		//record bounding boxs of primitives
		std::vector<Bound3f> primBounds(p.size());
		for (size_t i = 0; i < p.size(); i++)
			primBounds[i] = p[i]->worldBounds();

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

	//buildNode函数递归的构造kdTree
	void KdTreeAccel::buildNode(
		int* nodeNum,	//当前节点的index
		int depth,	//当前深度
		const int* pIndStart, //当前节点中的primitve数组对应的index数组
		int nPrimitives,	//当前节点中包含的primitive数目
		const Bound3f& nodeBounds,	//当前节点的包围盒
		const std::vector<Bound3f>& allPrimBounds,	//所有primitive对应的包围盒
		std::shared_ptr<BoundEdge[]> edge[3],
		int* prims0,
		int* prims1,
		int badRefine
	) {
		//如果当前已经分配的内存已满，分配双倍的内存并拷贝数据
		if (nextFreeNode == nAccelNode) {

			//分配双倍大小的内存
			int newNodeSize = std::max(512, 2 * nAccelNode);
			KdTreeNode* newArray = new KdTreeNode[newNodeSize];

			//如果原内存空间有数据，拷贝数据并释放原内存空间
			if (nAccelNode > 0) {
				std::memcpy(newArray, treeNodes, sizeof(KdTreeNode) * nAccelNode);
				delete[] treeNodes;
			}
			nAccelNode = newNodeSize;
			treeNodes = newArray;
		}

		//get next freeNode
		*nodeNum = nextFreeNode++;
		KdTreeNode* node = &treeNodes[*nodeNum];

		//leaf node
		if (nPrimitives < primsThreshold || depth >= maxDepth) {
			node->createLeaf(nPrimitives, pIndStart, &primIndices);
			return;
		}

		//interior node
		else {
			int bestAxis = -1;
			int bestOffset = -1;
			double bestCost = std::numeric_limits<double>::infinity();//loset cost found so far
			int defaultAxis = nodeBounds.maxExtent();//by default,choose the axis which has the max extent
			double unSplitCost = isectCost * nPrimitives;//

			int repeat = 0;
			//at most iterate 3 axis
			while (repeat < 3) {
				int axis = (defaultAxis + repeat) % 3;//switch to next axis
				int nEdges = 2 * nPrimitives;

				//generate nodeEdge array for current node
				for (int i = 0; i < nPrimitives; i++) {
					int primIndex = pIndStart[i];
					Bound3f pBox = allPrimBounds[primIndex];//get bounding box of ith primitive in current node
					edge[axis][2 * i] = { true, pBox.pMin[axis], primIndex };
					edge[axis][2 * i + 1] = { false, pBox.pMax[axis], primIndex };
				}

				//sort BoundEdges 
				std::sort(&edge[axis][0], &edge[axis][nEdges],
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
				for (int i = 0; i < nEdges; i++) {
					if (edge[axis][i].type == EdgeType::EdgeEnd) primsAbove--;

					//compute surface area of box blow and box above by adding up area of their six faces
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

					//compute posibilities of ray passing two subspaces 
					double pBelow = aBelow * invArea;
					double pAbove = aAbove * invArea;

					//compute split cost
					double eb = (primsBelow == 0 || primsAbove == 0) ? emptyBonus : 0;
					double splitCost = traversalCost +
						(1 - eb) * (pBelow * primsBelow * isectCost + pAbove * primsAbove * isectCost);
					if (splitCost < bestCost) {
						bestCost = splitCost;
						bestOffset = i;
					}
					if (edge[axis][i].type == EdgeType::EdgeStart)primsBelow++;
				}

				//find a good split 
				if (bestCost < unSplitCost) {
					bestAxis = axis;
					break;
				}

				//no good split, try next axis
				repeat++;
			}

			//Do not find a good split after iterating three axes
			if (bestCost > unSplitCost)badRefine++;
			if (bestAxis == -1 || (nPrimitives < 16 && bestCost > 4 * unSplitCost) || badRefine == 3) {
				node->createLeaf(nPrimitives, pIndStart, &primIndices);
				return;
			}

			//found a good split

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
			buildNode(&nextBelow, depth + 1, prims0, n0, boundBelow, allPrimBounds, edge, prims0, prims1 + nPrimitives, badRefine);
			buildNode(&nextAbove, depth + 1, prims1, n1, boundAbove, allPrimBounds, edge, prims0, prims1 + nPrimitives, badRefine);
			node->createInterior(bestAxis, nextAbove, edge[bestAxis][bestOffset].pos);
			return;
		}
	}

	bool KdTreeAccel::hit(
		const RayDifferential& r_in,
		double tMax
	)const {
		double t0, t1;
		if (worldBound.hit(r_in, &t0, &t1)) {
			if (t0 > tMax)
				return false;
			return true;
		}
		return false;
	}

	std::optional<SurfaceInteraction> KdTreeAccel::intersect(
		const RayDifferential& r_in,
		double tMax
	)const {
		HitInfo hitInfo;
		int closestIndex = 0;
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
						bool foundIntersection = prims[node.onePrimitive]->intersect(r_in, hitInfo, tMax);
						if (foundIntersection) {
							//incident ray hit primitive
							tMax = hitInfo.hitTime;//update tMin
							flag = true;
							closestIndex = node.onePrimitive;
						}
					}
					else {
						//a few primitives in this node
						for (int i = 0; i < primNum; i++) {
							int index = primIndices[node.indexOffset + i];
							bool foundIntersection = prims[index]->intersect(r_in, hitInfo, tMax);
							if (foundIntersection) {
								//incident ray hit this primitive
								tMax = hitInfo.hitTime;
								flag = true;
								closestIndex = index;
							}
						}
					}
				}
			}
		}
		if (flag == true) {
			SurfaceInteraction hitRecord = prims[closestIndex]->setInteractionProperty(hitInfo, r_in);
			return std::optional<SurfaceInteraction>(hitRecord);
		}
		return std::nullopt;
	}

}