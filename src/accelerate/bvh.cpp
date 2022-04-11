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

		//递归构建BVH树
		root = recursiveBuild(primInfo, 0, primInfo.size(), ordered);

		//用排好序的prim数组替换原数组
		prims.swap(ordered);
	}

	std::shared_ptr<BVHNode> BVHAccel::recursiveBuild(std::vector<PrimitiveInfo>& info, size_t start, size_t end,
		std::vector<std::shared_ptr<Primitive>>& ordered) {
		std::shared_ptr<BVHNode> currentNode = std::make_shared<BVHNode>();

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
			std::cout << "Leaf node generated, primitive count = " << nPrimitive << std::endl;
			return currentNode;
		}
		else if (nPrimitive == 2) {
			size_t middle = start + 1;
			std::shared_ptr<BVHNode> leftNode = recursiveBuild(info, start, middle, ordered);
			std::shared_ptr<BVHNode> rightNode = recursiveBuild(info, middle, end, ordered);
			currentNode->buildInterior(leftNode, rightNode);
			return currentNode;
		}
		//选择划分轴
		int axis = centroidBound.maxExtent();
		bool split = false;
		size_t middle;
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
			double minCost = std::numeric_limits<double>::max();
			int minBucket = 0;
			for (int i = 0; i < nBuckets - 1; i++) {
				if (buckets[i].nPrimitives == 0)
					continue;
				if (cost[i] < minCost) {
					minCost = cost[i];
					minBucket = i;
				}
			}

			//统计划分后左侧有多少primitive
			size_t leftPrimitives = 0;
			for (int i = 0; i <= minBucket; i++)
				leftPrimitives += buckets[i].nPrimitives;
			assert(leftPrimitives + start <= end);
			assert(leftPrimitives >= 0);

			//若需要进行划分(最小开销未出现在两边)
			if (leftPrimitives != 0 && leftPrimitives + start < end) {
				middle = start + leftPrimitives;
				split = true;
				break;
			}

			//沿着该坐标轴未找到合适的划分，尝试下一个坐标轴
			axis = (axis + 1) % 3;

		}

		//找到了开销更低的划分
		if (split) {
			//执行划分并递归求子节点
			std::shared_ptr<BVHNode> leftNode = recursiveBuild(info, start, middle, ordered);
			std::shared_ptr<BVHNode> rightNode = recursiveBuild(info, middle, end, ordered);
			currentNode->buildInterior(leftNode, rightNode);
			return currentNode;
		}
		//未找到开销更低的划分
		else {
			//生成叶子节点
			size_t firstOffset = ordered.size();

			for (int i = 0; i < nPrimitive; i++) {
				size_t primNum = info[i + start].primitiveIndex;
				ordered.push_back(prims[primNum]);
			}

			currentNode->buildLeaf(centroidBound, firstOffset, nPrimitive);
			std::cout << "Leaf node generated, primitive count = " << nPrimitive << std::endl;
			return currentNode;
		}
	}

	bool BVHAccel::hit(const Ray& r_in, double tMax)const {
		const int maxSize = 64;
		int size = 1;
		std::shared_ptr<BVHNode> nodes[maxSize];
		nodes[0] = root;
		int head = -1;
		int rear = 0;
		//遍历队列直至处理完所有node或者光线与primitive相交
		while (head != rear) {
			head = (head + 1) % 64;//处理下一个Node
			size--;
			double t0, t1;//光线与boundingbox相交的距离参数

			//测试当前节点是否与光线相交
			if (nodes[head]->box.hit(r_in, &t0, &t1)) {

				//光线与该节点的包围盒相交
				const std::shared_ptr<BVHNode>& node = nodes[head];
				if (node->children[0] != nullptr || node->children[1] != nullptr) {
					//该节点为中间节点,将子节点加入队列
					rear = (rear + 1) % 64;
					size++;
					assert(size < maxSize);
					nodes[rear] = node->children[0];
					rear = (rear + 1) % 64;
					size++;
					assert(size < maxSize);
					nodes[rear] = node->children[1];
				}
				else {
					//该节点为叶子节点,遍历子节点中的primitve
					for (size_t i = 0; i < node->nPrims; i++) {
						size_t index = node->firstPrimOffset + i;
						if (prims[index]->hit(r_in, tMax))
							return true;
					}
				}
			}
		}

		return false;
	}

	std::optional<SurfaceInteraction> BVHAccel::intersect(const Ray& ray, double tMax)const {
		const int maxSize = 64;
		int size = 1;
		std::shared_ptr<BVHNode> nodes[maxSize];
		nodes[0] = root;
		int head = -1;
		int rear = 0;
		double closet = tMax;
		std::optional<SurfaceInteraction> record;
	
		//遍历队列直至处理完所有node
		while (head != rear) {
			head = (head + 1) % 64;
			size--;
			//测试当前节点是否与光线相交
			double t0, t1;
			if (nodes[head]->box.hit(ray, &t0, &t1)) {
				//当前节点的包围盒与光线相交
	
				const std::shared_ptr<BVHNode>& node = nodes[head];
				if (node->children[0] != nullptr || node->children[1] != nullptr) {
					//该节点为中间节点
					rear = (rear + 1) % 64;
					size++;
					assert(size < maxSize);
					nodes[rear] = node->children[0];
					rear = (rear + 1) % 64;
					size++;
					assert(size < maxSize);
					nodes[rear] = node->children[1];
				}
				else {
					//该节点为叶子节点
					for (size_t i = 0; i < node->nPrims; i++) {
						//判断节点中的每一个primitive是否与光线相交
						size_t index = node->firstPrimOffset + i;
						std::optional<SurfaceInteraction> hitRecord = prims[index]->intersect(ray, closet);
						if (hitRecord.has_value()) {
							//该primitive与光线相交
							closet = hitRecord->t;
							record = hitRecord;
						}
					}
				}
			}
		}
	
		return record;
	}
}