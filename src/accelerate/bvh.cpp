#include"bvh.h"
#include<optional>
#include<vector>

namespace Raven {

	BVHAccel::BVHAccel(const std::vector<std::shared_ptr<Primitive>>& primitives, size_t maxSize = 1) :
		Accelerate(primitives), maxPrimInNode(maxSize) {

		//初始化primitiveInfo数组
		std::vector<PrimitiveInfo> primInfo(prims.size());
		for (size_t i = 0; i < primInfo.size(); i++) {
			primInfo[i] = PrimitiveInfo(prims[i]->worldBounds(), i);
		}

		std::vector<std::shared_ptr<Primitive>> ordered;

		size_t totalNodes = 0;
		//递归构建BVH树
		std::shared_ptr<BVHNode>root = recursiveBuild(primInfo, 0, primInfo.size(), ordered, totalNodes);

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
		size_t& totalNodes) {
		std::shared_ptr<BVHNode> currentNode = std::make_shared<BVHNode>();
		totalNodes++;

		//求当前node的boundingBox
		Bound3f centroidBound;
		for (size_t i = start; i < end; i++)
			centroidBound = Union(centroidBound, info[i].box);

		//如果primitive数目少于阈值，生成叶子节点
		int nPrimitive = end - start;
		if (nPrimitive == 1) {

			size_t firstOffset = ordered.size();

			for (int i = 0; i < nPrimitive; i++) {
				size_t primNum = info[start + i].primitiveIndex;
				ordered.push_back(prims[primNum]);
			}

			currentNode->buildLeaf(centroidBound, firstOffset, nPrimitive);
	//		std::cout << "Leaf node generated, primitive count = " << nPrimitive << std::endl;
			return currentNode;
		}

		//选择划分轴
		int axis = centroidBound.maxExtent();
		bool split = false;
		size_t middle = start;

		for (int i = 0; i < 3; i++) {
			//排序
			switch (axis) {
			case 0:
				std::sort(&info[start], &info[end - 1],
					[](const PrimitiveInfo& p0, const PrimitiveInfo& p1)->bool {
						return p0.centroid.x < p1.centroid.x;
					});
				break;
			case 1:
				std::sort(&info[start], &info[end - 1],
					[](const PrimitiveInfo& p0, const PrimitiveInfo& p1)->bool {
						return p0.centroid.y < p1.centroid.y;
					});
				break;
			case 2:
				std::sort(&info[start], &info[end - 1],
					[](const PrimitiveInfo& p0, const PrimitiveInfo& p1)->bool {
						return p0.centroid.z < p1.centroid.z;
					});
				break;
			}

			//SAH
			const int nBuckets = 16;
			SAHBucket buckets[nBuckets];

			//计算每个bucket中primitve的数目和boundingbox的大小
			for (size_t i = start; i < end; i++) {
				Bound3f box = info[i].box;
				int b = nBuckets * centroidBound.offset(info[i].centroid)[axis];
				if (b == nBuckets)b--;
				buckets[b].nPrimitives++;
				buckets[b].box = Union(buckets[b].box, box);
			}

			double leafCost = nPrimitive;

			//计算在每个bucket划分的开销
			double cost[nBuckets - 1];
			for (int i = 0; i < nBuckets - 1; i++) {
				//计算从第i个bucket中划分，两边的开销

				Bound3f left, right;
				int leftCount = 0, rightCount = 0;

				//划分后左边
				for (int j = 0; j <= i; j++) {
					left = Union(left, buckets[j].box);
					leftCount += buckets[j].nPrimitives;
				}

				//划分后右边
				for (int j = i + i; j < nBuckets; j++) {
					right = Union(right, buckets[j].box);
					rightCount += buckets[j].nPrimitives;
				}
				cost[i] = 0.125f + (Max(0.0, left.surfaceArea()) * leftCount
					+ Max(0.0, right.surfaceArea()) * rightCount)
					/ centroidBound.surfaceArea();
			}

			//寻找最小开销的划分
			double minCost = cost[0];
			int minBucket = 0;
			for (int i = 1; i < nBuckets - 1; i++) {
				if (buckets[i].nPrimitives == 0)
					continue;
				if (cost[i] < minCost) {
					minCost = cost[i];
					minBucket = i;
				}
			}

			if (minCost < leafCost) {

				//如果划分后的开销小于不划分的开销，统计划分后左侧有多少primitive
				size_t leftPrimitives = 0;
				for (int i = 0; i <= minBucket; i++)
					leftPrimitives += buckets[i].nPrimitives;
				assert(leftPrimitives + start <= end);
				assert(leftPrimitives >= 0);

				if (leftPrimitives != 0 && leftPrimitives + start < end) {
					middle = start + leftPrimitives;
					split = true;
					break;
				}
			}
			//沿着该坐标轴未找到合适的划分，尝试下一个坐标轴
			axis = (axis + 1) % 3;
		}

		//找到了开销更低的划分
		if (split) {
			//执行划分并递归求子节点
			std::shared_ptr<BVHNode> leftNode = recursiveBuild(info, start, middle, ordered, totalNodes);
			std::shared_ptr<BVHNode> rightNode = recursiveBuild(info, middle, end, ordered, totalNodes);
			currentNode->buildInterior(leftNode, rightNode, axis);
			return currentNode;
		}
		//未找到开销更低的划分
		//生成叶子节点
		size_t firstOffset = ordered.size();

		for (int i = 0; i < nPrimitive; i++) {
			size_t primNum = info[i + start].primitiveIndex;
			ordered.push_back(prims[primNum]);
		}

		currentNode->buildLeaf(centroidBound, firstOffset, nPrimitive);
	//	std::cout << "Leaf node generated, primitive count = " << nPrimitive << std::endl;
		return currentNode;

	}

	int BVHAccel::flattenTree(const std::shared_ptr<BVHNode>& node, int* offset) {
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
			node->nPrims = 0;
			flattenTree(node->children[0], offset);
			lnode->rightChild = flattenTree(node->children[1], offset);
		}
		return currentOffset;
	}

	bool BVHAccel::hit(const Ray& ray, double tMax)const {
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


	std::optional<SurfaceInteraction> BVHAccel::intersect(const Ray& ray, double tMax)const {
		bool hit = false;
		double closest = tMax;
		Vector3f invDir(1 / ray.dir.x, 1 / ray.dir.y, 1 / ray.dir.z);
		Vector3i dirIsNeg = Vector3i(invDir.x < 0, invDir.y < 0, invDir.z < 0);
		int nodesToVisite[4000];
		int currentIndex = 0;
		int offset = 0;
		SurfaceInteraction record;
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
						bool foundIntersection =
							prims[index]->intersect(ray, record, closest);
						if (foundIntersection) {
							hit = true;
							closest = record.t;
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
		if (hit)
			return std::optional<SurfaceInteraction>(record);
		else
			return std::nullopt;
	}
}