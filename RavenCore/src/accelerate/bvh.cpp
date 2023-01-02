#include<Raven/accelerate/bvh.h>
#include<optional>
#include<vector>

namespace Raven {

	BVHAccel::BVHAccel(const std::vector<std::shared_ptr<Primitive>>& primitives, size_t maxSize = 100) :
		Accelerate(primitives), maxPrimInNode(maxSize), maxDepth(0) {

		//��ʼ��primitiveInfo����
		std::vector<PrimitiveInfo> primInfo(prims.size());
		for (size_t i = 0; i < primInfo.size(); i++) {
			primInfo[i] = PrimitiveInfo(prims[i]->worldBounds(), i);
		}

		std::vector<std::shared_ptr<Primitive>> ordered;

		size_t totalNodes = 0;
		//�ݹ鹹��BVH��
		std::shared_ptr<BVHNode>root = recursiveBuild(primInfo, 0, primInfo.size(), ordered, totalNodes);
		std::cout << "BVH max depth: " << maxDepth << std::endl;
		//���ź����prim�����滻ԭ����
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

		//��ǰnode��boundingBox
		Bound3f bounds;
		for (size_t i = start; i < end; i++)
			bounds = Union(bounds, info[i].box);

		size_t nPrimitive = end - start;
		if (nPrimitive == 1) {
			//���ֻ��һ��ͼԪ��ֱ������Ҷ�ӽڵ�
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
			//�ж��ͼԪ�����Խ��л���

			//��������primitive���ĵ�İ�Χ��
			Bound3f centroidBound;
			for (size_t i = start; i < end; i++) {
				centroidBound = Union(centroidBound, info[i].centroid);
			}

			//ѡ�񻮷���
			int axis = centroidBound.maxExtent();
			size_t middle = (start + end) / 2;

			//�������ͼԪ�����ĵ㶼��ͬһ���㣬����Ҷ�ӽڵ�
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

			//�ж��ͼԪ�����ĵ㲻�غϣ����Ի���
			else {
				if (nPrimitive <= 2) {
					//ִ�л��ֲ��ݹ����ӽڵ�
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

					//����ÿ��bucket��primitve����Ŀ��boundingbox�Ĵ�С
					for (size_t i = start; i < end; i++) {
						Bound3f box = info[i].box;
						int b = nBuckets * centroidBound.offset(info[i].centroid)[axis];
						if (b == nBuckets)b--;
						assert(b >= 0);
						assert(b < nBuckets);
						buckets[b].nPrimitives++;
						buckets[b].box = Union(buckets[b].box, box);
					}

					//������ÿ��bucket���ֵĿ���
					double cost[nBuckets - 1];
					for (int i = 0; i < nBuckets - 1; i++) {
						//����ӵ�i��bucket�л��֣����ߵĿ���

						Bound3f left, right;
						int leftCount = 0, rightCount = 0;

						//���ֺ����
						for (int j = 0; j <= i; j++) {
							if (buckets[j].nPrimitives > 0) {
								left = Union(left, buckets[j].box);
								leftCount += buckets[j].nPrimitives;
							}
						}

						//���ֺ��ұ�
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

					//Ѱ����С�����Ļ���
					double minCost = cost[0];
					int minBucket = 0;
					for (int i = 1; i < nBuckets - 1; i++) {
						if (cost[i] < minCost) {
							minCost = cost[i];
							minBucket = i;
						}
					}

					double leafCost = nPrimitive;


					//������ֵĿ�����С,����С�Ŀ���������ͼԪ
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

					//�����ֵĿ�����С	
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

				//ִ�л��ֲ��ݹ����ӽڵ�
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
		//ȡ��Ҫ��ֵ��linearNode
		LinearBVHNode* lnode = &linearTree[*offset];
		lnode->box = node->box;
		int currentOffset = (*offset)++;

		//ԭ�ڵ�ΪҶ�ӽڵ�
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
				//�������Χ���ཻ

				if (node->nPrims > 0) {
					//�ýڵ�ΪҶ�ӽڵ�

					for (size_t i = 0; i < node->nPrims; i++) {
						//��ڵ��ڵ�Primitive��
						size_t index = node->firstOffset + i;
						if (prims[index]->hit(ray, tMax))
							return true;
					}

					if (offset == 0)break;
					currentIndex = nodesToVisite[--offset];
				}
				else {
					//�ýڵ�Ϊ�м�ڵ�
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
				//����δ�뵱ǰ�ڵ�İ�Χ���ཻ��������һ���ڵ�
				if (offset == 0)break;	//û����Ҫ���ʵĽڵ�
				currentIndex = nodesToVisite[--offset];//ȡ����һ����Ҫ���ʵĽڵ�
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
				//�������Χ���ཻ

				if (node->nPrims > 0) {
					//�ýڵ�ΪҶ�ӽڵ�

					for (size_t i = 0; i < node->nPrims; i++) {
						//��ڵ��ڵ�Primitive��
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
					//�ýڵ�Ϊ�м�ڵ�
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
				//����δ�뵱ǰ�ڵ�İ�Χ���ཻ��������һ���ڵ�
				if (offset == 0)break;	//û����Ҫ���ʵĽڵ�
				currentIndex = nodesToVisite[--offset];//ȡ����һ����Ҫ���ʵĽڵ�
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

