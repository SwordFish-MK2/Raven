#include <Raven/core/accelerate.h>

namespace Raven {

bool PrimitiveList::hit(const RayDifferential& r_in) const {
  for (size_t i = 0; i < prims.size(); i++) {
    if (prims[i]->hit(r_in))
      return true;
  }
  return false;
}

std::optional<SurfaceInteraction> PrimitiveList::intersect(
    const RayDifferential& r_in) const {
  bool    flag = false;
  HitInfo hitInfo;
  int     index;

  // 求最近的交点
  for (int i = 0; i < prims.size(); i++) {
    bool foundIntersection = prims[i]->intersect(r_in, hitInfo);
    if (foundIntersection) {
      flag  = true;
      index = i;
    }
  }

  // 如果相交
  if (flag) {
    // 生成最近交点的SurfaceInteraction
    SurfaceInteraction hitRecord =
        prims[index]->setInteractionProperty(hitInfo, r_in);
    return hitRecord;
  }

  else
    return std::nullopt;
}
}  // namespace Raven