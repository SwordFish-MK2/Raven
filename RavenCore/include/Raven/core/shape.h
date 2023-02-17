#ifndef _RAVEN_CORE_SHAPE_H_
#define _RAVEN_CORE_SHAPE_H_

#include <Raven/core/aabb.h>
#include <Raven/core/base.h>
#include <Raven/core/math.h>
#include <Raven/core/object.h>
#include <Raven/core/ray.h>
#include <Raven/core/transform.h>

#include <optional>

namespace Raven {
enum PrimType {
  PTriangle,
  PSphere
};

///< summary>
/// Geometric info of a surface interaction
///</summary>
struct HitInfo {
  Vector3f wo;    // incident ray direction
  Point3f  pHit;  // intersection point

  HitInfo() {}

  void setInfo(const Point3f& p, const Vector3f dir) {
    pHit = p;
    wo   = dir;
  }
};

/// <summary>
/// Shape interface, all geometrics must inherit this class
/// </summary>
class Shape : public RavenObject {
 protected:
  Ref<Transform> localToWorld;
  Ref<Transform> worldToLocal;

 public:
  Shape(const Ref<Transform>& LTW, const Ref<Transform>& WTL)
      : localToWorld(LTW), worldToLocal(WTL) {}

  // intersect incident ray with shape and return whether shape is hitted
  virtual bool hit(const Ray& r_in) const = 0;

  // intersect incident ray with shape and compute surfaceIntersection
  virtual bool intersect(const Ray& r_in, HitInfo& hitInfo) const = 0;

  virtual SurfaceInteraction getGeoInfo(const Point3f& hitInfo) const = 0;

  // return the bounding box of shape in local space
  virtual Bound3f localBound() const = 0;

  // return the bounding box of shape in world space
  virtual Bound3f worldBound() const = 0;

  // return area of shape surface
  virtual double area() const = 0;

  // 均匀的在几何体表面采样，返回几何体表面的一个点，pdf为对面积积分的pdf
  virtual std::tuple<SurfaceInteraction, double> sample(
      const Point2f& rand) const = 0;

  // 给定空间中的一个点，在几何体体表面采样一个点，pdf为对立体角的积分
  virtual std::tuple<SurfaceInteraction, double> sample(
      const Interaction& ref,
      const Point2f&     rand) const;

  // 在几何体上均匀采样的pdf，对面积积分
  virtual double pdf(const Interaction&) const { return 1 / area(); }

  // 在空间中给定点采样的pdf，对立体角积分
  virtual double pdf(const Interaction& inter, const Vector3f& wi) const;
};
}  // namespace Raven

#endif