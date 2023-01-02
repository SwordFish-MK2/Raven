#include<Raven/accelerate/bvh.h>
#include<optional>
#include<vector>

namespace Raven {

	BVHAccel::BVHAccel(const std::vector<std::shared_ptr<Primitive>>& primitives, size_t maxSize = 100) :
		Accelerate(primitives), maxPrimInNode(maxSize), maxDepth(0) {

		//初始化primitiveInfo数组
		std::vector<PrimitiveInfo> primInfo(prims.size());
		for (size_t i = 0; i < primInfo.size(); i++) {
			primInfo[i] = PrimitiveInfo(prims[i]->worldBounds(), i);
		}

		std::vector<std::shared_ptr<Primitive>> ordered;

		size_t totalNodes = 0;
		//递归构建BVH树
		std::shared_ptr<BVHNode>root = recursiveBuild(primInfo, 0, primInfo.size(), ordered, totalNodes);
		std::cout << "BVH max depth: " << maxDepth << std::endl;
		//用排好序的prim数组替换原数组
		prims.swap(ordered);

		linearTree = std::vector<LinearBVHNode>(totalNodes);
		int offset = 0;
		flattenTree(root, &offset);
		std::cout << "BVH generation complete, total node number:" << totalNodes << std::endl;
	}

	std::shared_ptr<BVHNode> BVHAccel::recursiveBuild(
		std::vector<PrimitiveInfo>& info,
		size_t start,
		size_t end,
		std::vector<std::shared_ptr<Primitive>>& ordered,
		size_t& totalNodes, int depth) {

		depth++;
		if (depth > maxDepth) {
			maxDepth = depth;
		}

		std::shared_ptr<BVHNode> currentNode = std::make_shared<BVHNode>();
		totalNodes++;

		//求当前node的boundingBox
		Bound3f bounds;
		for (size_t i = start; i < end; i++)
			bounds = Union(bounds, info[i].box);

		size_t nPrimitive = end - start;
		if (nPrimitive == 1) {
			//如果只有一个图元，直接生成叶子节点
			size_t firstOffset = ordered.size();
			for (int i = start; i < end; i++) {
				size_t primNum = info[i].primitiveIndex;
				ordered.push_back(prims[primNum]);
			}

			currentNode->buildLeaf(bounds, firstOffset, nPrimitive);
			depth--;
			return currentNode;
		}
		else {
			//有多个图元，尝试进行划分

			//计算所有primitive中心点的包围盒
			Bound3f centroidBound;
			for (size_t i = start; i < end; i++) {
				centroidBound = Union(centroidBound, info[i].centroid);
			}

			//选择划分轴
			int axis = centroidBound.maxExtent();
			size_t middle = (start + end) / 2;

			//如果所有图元的中心点都在同一个点，生成叶子节点
			if (centroidBound.pMax[axis] == centroidBound.pMin[axis]) {
				size_t firstOffset = ordered.size();
				for (int i = start; i < end; i++) {
					size_t primNum = info[i].primitiveIndex;
					ordered.push_back(prims[primNum]);
				}
				currentNode->buildLeaf(bounds, firstOffset, nPrimitive);
				depth--;
				return currentNode;
			}

			//有多个图元且中心点不重合，尝试划分
			else {
				if (nPrimitive <= 2) {
					//执行划分并递归求子节点
					middle = (start + end) / 2;
					std::nth_element(
						&info[start],
						&info[middle],
						&info[end - 1] + 1,
						[axis](const PrimitiveInfo& a, const PrimitiveInfo& b) {
							return a.centroid[axis] < b.centroid[axis]; });
				}
				else {
					//SAH
					const int nBuckets = 12;
					SAHBucket buckets[nBuckets];

					//计算每个bucket中primitve的数目和boundingbox的大小
					for (size_t i = start; i < end; i++) {
						Bound3f box = info[i].box;
						int b = nBuckets * centroidBound.offset(info[i].centroid)[axis];
						if (b == nBuckets)b--;
						assert(b >= 0);
						assert(b < nBuckets);
						buckets[b].nPrimitives++;
						buckets[b].box = Union(buckets[b].box, box);
					}

					//计算在每个bucket划分的开销
					double cost[nBuckets - 1];
					for (int i = 0; i < nBuckets - 1; i++) {
						//计算从第i个bucket中划分，两边的开销

						Bound3f left, right;
						int leftCount = 0, rightCount = 0;

						//划分后左边
						for (int j = 0; j <= i; j++) {
							if (buckets[j].nPrimitives > 0) {
								left = Union(left, buckets[j].box);
								leftCount += buckets[j].nPrimitives;
							}
						}

						//划分后右边
						for (int j = i + 1; j < nBuckets; j++) {
							if (buckets[j].nPrimitives > 0) {
								right = Union(right, buckets[j].box);
								rightCount += buckets[j].nPrimitives;
							}
						}

						cost[i] = 0.125 +
							(Max(0.0, left.surfaceArea()) * leftCount
								+ Max(0.0, right.surfaceArea()) * rightCount)
							/ centroidBound.surfaceArea();
					}

					//寻找最小开销的划分
					double minCost = cost[0];
					int minBucket = 0;
					for (int i = 1; i < nBuckets - 1; i++) {
						if (cost[i] < minCost) {
							minCost = cost[i];
							minBucket = i;
						}
					}

					double leafCost = nPrimitive;


					//如果划分的开销更小,在最小的开销处划分图元
					if (nPrimitive > maxPrimInNode || minCost < leafCost) {
						PrimitiveInfo* pmid = std::partition(
							&info[start], &info[end - 1] + 1,
							[=](const PrimitiveInfo& pi) {
								int b = nBuckets *
								centroidBound.offset(pi.centroid)[axis];
						if (b == nBuckets) b = nBuckets - 1;
						assert(b >= 0);
						assert(b < nBuckets);
						return b <= minBucket;
							});
						middle = pmid - &info[0];
					}

					//不划分的开销最小	
					else {
						size_t firstOffset = ordered.size();
						for (int i = 0; i < nPrimitive; i++) {
							size_t primNum = info[start + i].primitiveIndex;
							ordered.push_back(prims[primNum]);
						}

						currentNode->buildLeaf(bounds, firstOffset, nPrimitive);
						depth--;
						return currentNode;
					}
				}

				//执行划分并递归求子节点
				std::shared_ptr<BVHNode> leftNode = recursiveBuild(info, start, middle, ordered, totalNodes, depth);
				std::shared_ptr<BVHNode> rightNode = recursiveBuild(info, middle, end, ordered, totalNodes, depth);
				currentNode->buildInterior(leftNode, rightNode, axis);
			}
		}
		depth--;
		return currentNode;
	}

	int BVHAccel::flattenTree(
		const std::shared_ptr<BVHNode>& node,
		int* offset) {
		//取出要赋值的linearNode
		LinearBVHNode* lnode = &linearTree[*offset];
		lnode->box = node->box;
		int currentOffset = (*offset)++;

		//原节点为叶子节点
		if (node->nPrims > 0) {
			lnode->firstOffset = node->firstPrimOffset;
			lnode->nPrims = node->nPrims;
		}
		else {
			lnode->axis = node->splitAxis;
			lnode->nPrims = 0;
			flattenTree(node->children[0], offset);
			lnode->rightChild = flattenTree(node->children[1], offset);
		}
		return currentOffset;
	}

	bool BVHAccel::hit(const RayDifferential& ray, double tMax)const {
		Vector3f invDir(1 / ray.dir.x, 1 / ray.dir.y, 1 / ray.dir.z);
		Vector3i dirIsNeg = Vector3i(invDir.x < 0, invDir.y < 0, invDir.z < 0);
		int nodesToVisite[64];
		int currentIndex = 0;
		int offset = 0;
		while (1) {
			const LinearBVHNode* node = &linearTree[currentIndex];
			double t0, t1;
			if (node->box.hit(ray, &t0, &t1)) {
				//光线与包围盒相交

				if (node->nPrims > 0) {
					//该节点为叶子节点

					for (size_t i = 0; i < node->nPrims; i++) {
						//与节点内的Primitive求交
						size_t index = node->firstOffset + i;
						if (prims[index]->hit(ray, tMax))
							return true;
					}

					if (offset == 0)break;
					currentIndex = nodesToVisite[--offset];
				}
				else {
					//该节点为中间节点
					if (dirIsNeg[node->axis]) {
						nodesToVisite[offset++] = currentIndex + 1;
						currentIndex = node->rightChild;
					}
					else {
						nodesToVisite[offset++] = node->rightChild;
						currentIndex = currentIndex + 1;
					}
				}
			}
			else {
				//光线未与当前节点的包围盒相交，访问下一个节点
				if (offset == 0)break;	//没有需要访问的节点
				currentIndex = nodesToVisite[--offset];//取得下一个需要访问的节点
			}
		}
		return false;
	}

	std::optional<SurfaceInteraction> BVHAccel::intersect(const RayDifferential& ray)const {
		bool hit = false;
		std::vector<int>flags(linearTree.size());
		Vector3f invDir(1 / ray.dir.x, 1 / ray.dir.y, 1 / ray.dir.z);
		int dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };
		int nodesToVisite[64];
		int currentIndex = 0;
		int offset = 0;
		int primHited = 0;
		HitInfo record;
		int counter = 0;
		while (true) {
			counter++;
			const LinearBVHNode* node = &linearTree[currentIndex];
			double t0, t1;

			if (node->box.hit(ray, &t0, &t1)) {
				//光线与包围盒相交

				if (node->nPrims > 0) {
					//该节点为叶子节点

					for (size_t i = 0; i < node->nPrims; i++) {
						//与节点内的Primitive求交
						size_t index = node->firstOffset + i;
						bool foundIntersection =
							prims[index]->intersect(ray, record);
						if (foundIntersection) {
							hit = true;
							primHited = index;
						}
					}

					if (offset == 0)break;
					currentIndex = nodesToVisite[--offset];
				}
				else {
					//该节点为中间节点
					if (dirIsNeg[node->axis]) {
						nodesToVisite[offset++] = currentIndex + 1;
						currentIndex = node->rightChild;
					}
					else {
						nodesToVisite[offset++] = node->rightChild;
						currentIndex = currentIndex + 1;
					}
				}
			}
			else {
				//光线未与当前节点的包围盒相交，访问下一个节点
				if (offset == 0)break;	//没有需要访问的节点
				currentIndex = nodesToVisite[--offset];//取得下一个需要访问的节点
			}

		}
		if (hit) {
			SurfaceInteraction hitRecord = prims[primHited]->setInteractionProperty(record, ray);

			return std::optional<SurfaceInteraction>(hitRecord);
		}

		//else
		return std::nullopt;
	}

}

