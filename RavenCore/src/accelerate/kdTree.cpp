#include <Raven/accelerate/kdTree.h>

#include <cstring>
namespace Raven {
KdTreeAccel::KdTreeAccel(const std::vector<std::shared_ptr<Primitive>>& p,
                         int                                            maxD,
                         int                                            it,
                         int                                            tt,
                         double                                         eb,
                         int primsThreshold)
    : Accelerate(p),
      maxDepth(maxD),
      isectCost(it),
      traversalCost(tt),
      emptyBonus(eb),
      primsThreshold(primsThreshold) {
  // compute a reasonable maxDepth if its not defined by user
  if (maxDepth <= 0)
    maxDepth = std::round(8 + 1.3 * log2l(prims.size()));

  // record bounding boxs of primitives
  std::vector<Bound3f> primBounds(p.size());
  for (size_t i = 0; i < p.size(); i++) primBounds[i] = p[i]->worldBounds();

  // generate primitive indices
  std::vector<int> primNumbers(prims.size());
  for (size_t i = 0; i < p.size(); i++) primNumbers[i] = i;

  // allocate memory for BoundEdges
  std::shared_ptr<BoundEdge[]> edge[3];
  for (int i = 0; i < 3; i++) edge[i].reset(new BoundEdge[2 * prims.size()]);
  std::shared_ptr<int[]> prims0(new int[prims.size()]);
  std::shared_ptr<int[]> prims1(new int[(maxDepth + 1) * prims.size()]);

  // build root node
  std::cout << "Kd-tree start building node, primitive number: " << prims.size()
            << std::endl;
  int root = 0;
  buildNode(&root, maxD, &primNumbers[0], prims.size(), bounds, primBounds,
            edge, prims0.get(), prims1.get(), 0);
  std::cout << "Kd-tree build finish, Number of tree nodes: " << nextFreeNode
            << std::endl;
}

// buildNode function recursively build kd-tree
void KdTreeAccel::buildNode(
    int* nodeNum,          // index of current node
    int  depth,            // current node depth
    const int* pIndStart,  // array of indices of primitives inside current ndoe
    int            nPrimitives,  // number of primitives inside current node
    const Bound3f& nodeBounds,   // bounding box of current node
    const std::vector<Bound3f>&  allPrimBounds,  // bounds of all primitives
    std::shared_ptr<BoundEdge[]> edge[3],
    int*                         prims0,
    int*                         prims1,
    int                          badRefine) {
  // current allocated memory is full, allocate double-sized memory and copy the
  // nodes generated.
  if (nextFreeNode == nAccelNode) {
    // allocate double-sized memory
    int         newNodeSize = std::max(512, 2 * nAccelNode);
    KdTreeNode* newArray    = new KdTreeNode[newNodeSize];

    // copy nodes
    if (nAccelNode > 0) {
      std::memcpy(newArray, treeNodes, sizeof(KdTreeNode) * nAccelNode);
      delete[] treeNodes;
    }
    nAccelNode = newNodeSize;
    treeNodes  = newArray;
  }

  // get next freeNode
  *nodeNum         = nextFreeNode++;
  KdTreeNode* node = &treeNodes[*nodeNum];

  // leaf node
  if (nPrimitives < primsThreshold || depth >= maxDepth) {
    node->createLeaf(nPrimitives, pIndStart, &primIndices);
    return;
  }

  // interior node
  else {
    int    bestAxis   = -1;
    int    bestOffset = -1;
    double bestCost =
        std::numeric_limits<double>::infinity();  // loset cost found so far
    int defaultAxis = nodeBounds.maxExtent();     // by default,choose the axis
                                               // which has the max extent
    double unSplitCost = isectCost * nPrimitives;  //

    int repeat = 0;
    // at most iterate 3 axis
    while (repeat < 3) {
      int axis   = (defaultAxis + repeat) % 3;  // switch to next axis
      int nEdges = 2 * nPrimitives;

      // generate nodeEdge array for current node
      for (int i = 0; i < nPrimitives; i++) {
        int     primIndex = pIndStart[i];
        Bound3f pBox = allPrimBounds[primIndex];  // get bounding box of ith
                                                  // primitive in current node
        edge[axis][2 * i]     = {true, pBox.pMin[axis], primIndex};
        edge[axis][2 * i + 1] = {false, pBox.pMax[axis], primIndex};
      }

      // sort BoundEdges
      std::sort(&edge[axis][0], &edge[axis][nEdges],
                [](const BoundEdge& e0, const BoundEdge& e1) -> bool {
                  if (e0.pos == e1.pos)
                    return (int)e0.type < (int)e1.type;
                  else
                    return e0.pos < e1.pos;
                });

      // compute cost for unsplit case
      int    primsAbove = nPrimitives;
      int    primsBelow = 0;
      double invArea    = 1.0 / nodeBounds.surfaceArea();

      // iterate over candidate split position, compute cost
      for (int i = 0; i < nEdges; i++) {
        if (edge[axis][i].type == EdgeType::EdgeEnd)
          primsAbove--;

        // compute surface area of box blow and box above by adding up area of
        // their six faces
        int      otherAxis0 = (axis + 1) % 3;
        int      otherAxis1 = (axis + 2) % 3;
        Vector3f d          = nodeBounds.pMax - nodeBounds.pMin;
        double   splitPos   = edge[axis][i].pos;
        double   aBelow =
            2 * ((d[otherAxis0] * d[otherAxis1]) +
                 (splitPos - nodeBounds.pMin[axis]) * d[otherAxis1] +
                 (splitPos - nodeBounds.pMin[axis]) * d[otherAxis0]);
        double aAbove =
            2 * ((d[otherAxis0] * d[otherAxis1]) +
                 (nodeBounds.pMax[axis] - splitPos) * d[otherAxis1] +
                 (nodeBounds.pMax[axis] - splitPos) * d[otherAxis0]);

        // compute posibilities of ray passing two subspaces
        double pBelow = aBelow * invArea;
        double pAbove = aAbove * invArea;

        // compute split cost
        double eb = (primsBelow == 0 || primsAbove == 0) ? emptyBonus : 0;
        double splitCost =
            traversalCost + (1 - eb) * (pBelow * primsBelow * isectCost +
                                        pAbove * primsAbove * isectCost);
        if (splitCost < bestCost) {
          bestCost   = splitCost;
          bestOffset = i;
        }
        if (edge[axis][i].type == EdgeType::EdgeStart)
          primsBelow++;
      }

      // find a good split
      if (bestCost < unSplitCost) {
        bestAxis = axis;
        break;
      }

      // no good split, try next axis
      repeat++;
    }

    // Do not find a good split after iterating three axes
    if (bestCost > unSplitCost)
      badRefine++;
    if (bestAxis == -1 || (nPrimitives < 16 && bestCost > 4 * unSplitCost) ||
        badRefine == 3) {
      node->createLeaf(nPrimitives, pIndStart, &primIndices);
      return;
    }

    // found a good split

    // classify primitives
    int n0 = 0, n1 = 0;
    for (int i = 0; i < bestOffset; ++i)
      if (edge[bestAxis][i].type == EdgeType::EdgeStart)
        prims0[n0++] = edge[bestAxis][i].prim;
    for (int i = bestOffset + 1; i < 2 * nPrimitives; i++)
      if (edge[bestAxis][i].type == EdgeType::EdgeEnd)
        prims1[n1++] = edge[bestAxis][i].prim;

    // compute bounds of child nodes
    Bound3f boundBelow        = nodeBounds;
    Bound3f boundAbove        = nodeBounds;
    boundBelow.pMax[bestAxis] = edge[bestAxis][bestOffset].pos;
    boundAbove.pMin[bestAxis] = edge[bestAxis][bestOffset].pos;
    // build child node and inite interior node for this node
    int nextBelow, nextAbove;
    buildNode(&nextBelow, depth + 1, prims0, n0, boundBelow, allPrimBounds,
              edge, prims0, prims1 + nPrimitives, badRefine);
    buildNode(&nextAbove, depth + 1, prims1, n1, boundAbove, allPrimBounds,
              edge, prims0, prims1 + nPrimitives, badRefine);
    node->createInterior(bestAxis, nextAbove, edge[bestAxis][bestOffset].pos);
    return;
  }
}

bool KdTreeAccel::hit(const RayDifferential& ray) const {
  // the ray misses overall bounding box of kdTree
  Float tMax, tMin;
  if (!bounds.hit(ray, &tMin, &tMax))
    return false;

  // prepare a stack to store nodes to visite
  const int     maxToDo = 64;
  NodeToProcess todo[maxToDo];
  int           todoPos = 0;

  // iterate kdtree to find closet intersection
  Float invDir[3] = {1.0 / ray.dir[0], 1.0 / ray.dir[1], 1.0 / ray.dir[2]};
  const KdTreeNode* currentNode = &treeNodes[0];
  while (currentNode != nullptr) {
    // current node is interior node, same as KdTreeAccel::intersect function
    if (!currentNode->isLeaf()) {
      int   splitAxis   = currentNode->getAxis();
      Float tSplitPlane = (currentNode->getSplitPos() - ray.origin[splitAxis]) *
                          invDir[splitAxis];

      // compute which child node the ray hit first
      const KdTreeNode* firstNode;
      const KdTreeNode* secondNode;
      bool belowFirst = (ray.origin[splitAxis] < currentNode->getSplitPos() ||
                         (ray.origin[splitAxis] == currentNode->getSplitPos() &&
                          ray.dir[splitAxis] <= 0));
      if (belowFirst) {
        firstNode  = currentNode + 1;
        secondNode = &treeNodes[currentNode->getAboveChild()];
      } else {
        firstNode  = &treeNodes[currentNode->getAboveChild()];
        secondNode = currentNode + 1;
      }

      // the ray only hits first child node
      if (tMax < tSplitPlane || tSplitPlane <= 0)
        currentNode = firstNode;

      // the ray only hits second child node
      else if (tMin > tSplitPlane)
        currentNode = secondNode;

      // hit both
      else {
        // press second node into todo stack
        currentNode        = firstNode;
        todo[todoPos].node = secondNode;
        todo[todoPos].tMin = tSplitPlane;
        todo[todoPos].tMax = tMax;

        // set properties for next node
        todoPos++;
        tMax        = tSplitPlane;
        currentNode = firstNode;
      }
    }

    // current node is leaf node
    // iterate primitives inside current node, whenever found a hit, return true
    else {
      int nPrimitives = currentNode->getPrimNum();
      if (nPrimitives == 1) {
        const auto& p = prims[currentNode->onePrimitive];
        if (p->hit(ray))
          return true;
      } else {
        for (int i = 0; i < nPrimitives; i++) {
          int         index = currentNode->splitPos + i;
          const auto& p     = prims[index];
          if (p->hit(ray))
            return true;
        }
      }

      // get next node to process
      if (todoPos > 0) {
        todoPos--;
        currentNode = todo[todoPos].node;
        tMin        = todo[todoPos].tMin;
        tMax        = todo[todoPos].tMax;
      }

      // after iterating every node in KdTree but still finding no hit, return
      // false
      else
        return false;
    }
  }

  // by default, return false
  return false;
}

std::optional<SurfaceInteraction> KdTreeAccel::intersect(
    const RayDifferential& ray) const {
  // the ray misses overall bounding box of kdTree
  Float tMax, tMin;
  if (!bounds.hit(ray, &tMin, &tMax))
    return std::nullopt;

  // prepare a stack to store nodes to visite
  const int     maxToDo = 64;
  NodeToProcess todo[maxToDo];
  int           toDoPos = 0;

  // iterate kdtree to find closet intersection
  bool    hit = false;
  HitInfo info;
  int     primHited = 0;
  Float   invDir[3] = {1.0 / ray.dir[0], 1.0 / ray.dir[1], 1.0 / ray.dir[2]};
  const KdTreeNode* node = &treeNodes[0];
  while (node) {
    // break if a closer intersection is found
    if (ray.tMax < tMin)
      break;

    // current node is interior node
    if (!node->isLeaf()) {
      int   splitAxis = node->getAxis();
      Float tSplitPlane =
          (node->getSplitPos() - ray.origin[splitAxis]) * invDir[splitAxis];

      // compute which child node the ray hit first
      const KdTreeNode* firstNode;
      const KdTreeNode* secondNode;
      bool belowFirst = (ray.origin[splitAxis] < node->getSplitPos() ||
                         (ray.origin[splitAxis] == node->getSplitPos() &&
                          ray.dir[splitAxis] <= 0));
      if (belowFirst) {
        firstNode  = node + 1;
        secondNode = &treeNodes[node->getAboveChild()];
      } else {
        firstNode  = &treeNodes[node->getAboveChild()];
        secondNode = node + 1;
      }

      // the ray only hits first child node
      if (tMax < tSplitPlane || tSplitPlane <= 0)
        node = firstNode;

      // the ray only hits second child node
      else if (tMin > tSplitPlane)
        node = secondNode;

      // hit both
      else {
        // press second node into todo stack
        node               = firstNode;
        todo[toDoPos].node = secondNode;
        todo[toDoPos].tMin = tSplitPlane;
        todo[toDoPos].tMax = tMax;

        // set properties for next node
        toDoPos++;
        tMax = tSplitPlane;
        node = firstNode;
      }
    }

    // current node is leaf node
    else {
      int nPrimitives = node->getPrimNum();

      // perform ray-primitive intersection test for primitives inside current
      // leaf node
      if (nPrimitives == 1) {
        const auto& p = prims[node->onePrimitive];
        if (p->intersect(ray, info) == true) {
          hit       = true;
          primHited = node->onePrimitive;
        }
      } else {
        for (int i = 0; i < nPrimitives; i++) {
          int         index = node->indexOffset + i;
          const auto& p     = prims[index];
          if (p->intersect(ray, info) == true) {
            hit       = true;
            primHited = index;
          }
        }
      }

      // get next node to process
      if (toDoPos > 0) {
        toDoPos--;
        node = todo[toDoPos].node;
        tMin = todo[toDoPos].tMin;
        tMax = todo[toDoPos].tMax;
      } else
        break;
    }
  }

  if (hit)
    return prims[primHited]->setInteractionProperty(info, ray);
  else
    return std::nullopt;
}
}  // namespace Raven