#ifndef _RAVEN_CORE_ACCELERATE_H_
#define _RAVEN_CORE_ACCELERATE_H_

#include <Raven/core/base.h>
#include <Raven/core/interaction.h>
#include <Raven/core/object.h>
#include <Raven/core/primitive.h>

#include <optional>

namespace Raven {
class Accelerate : public RavenObject {
 public:
  Accelerate(const std::vector<std::shared_ptr<Primitive>>& prims)
      : prims(prims) {
    for (int i = 0; i < prims.size(); i++)
      bounds = Union(prims[i]->worldBounds(), bounds);
  }

  virtual bool hit(const RayDifferential& r_in) const = 0;

  virtual std::optional<SurfaceInteraction> intersect(
      const RayDifferential& r_in) const = 0;

  virtual Bound3f worldBounds() const { return bounds; }

  std::vector<std::shared_ptr<Primitive>> prims;

 protected:
  Bound3f bounds;
};

class PrimitiveList : public Accelerate {
 public:
  PrimitiveList(const std::vector<std::shared_ptr<Primitive>>& prims)
      : Accelerate(prims) {}

  bool hit(const RayDifferential& r_in) const override;

  std::optional<SurfaceInteraction> intersect(
      const RayDifferential& r_in) const override;
};

enum class AccelType {
  List,
  KdTree,
  BVH
};
}  // namespace Raven

#endif
