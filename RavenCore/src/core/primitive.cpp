#include <Raven/core/medium.h>
#include <Raven/core/primitive.h>

namespace Raven {
bool GeometryPrimitive::hit(const Ray& r_in) const {
  // perform ray-geometry intersection test
  if (!shape_ptr->hit(r_in)) { return false; }
  return true;
}

bool GeometryPrimitive::intersect(const Ray& ray, HitInfo& record) const {
  // 判断光线是否与几何体相交
  bool foundIntersection = shape_ptr->intersect(ray, record);
  return foundIntersection;
}

SurfaceInteraction GeometryPrimitive::setInteractionProperty(
    const HitInfo&         hitInfo,
    const RayDifferential& ray) const {
  // 取得交点的几何信息
  SurfaceInteraction record = shape_ptr->getGeoInfo(hitInfo.pHit);
  record.computeDifferential(ray);

  // 配置材质信息
  mate_ptr->computeScarttingFunctions(record, true);

  // 配置光源信息
  if (light_ptr.get() != nullptr) {
    record.hitLight = true;
    record.light    = this->getAreaLight();
  } else {
    record.hitLight = false;
  }

  // get light direction
  record.wo = hitInfo.wo;

  // set medium interface
  if (mediumInterface.isMediumTransition())
    record.mediumInterface = mediumInterface;
  else
    record.mediumInterface = MediumInterface(ray.medium);

  return record;
}

Bound3f GeometryPrimitive::worldBounds() const {
  return shape_ptr->worldBound();
}

bool TransformedPrimitive::hit(const Ray& r_in) const {
  if (!primToWorld || !worldToPrim || !prim) return false;
  // transform the incident ray to primitive space then perform ray intersection
  // test
  Ray transformedRay = Inverse(*primToWorld)(r_in);
  return prim->hit(r_in);
}

bool TransformedPrimitive::intersect(const Ray &ray, HitInfo &info)const{
  return prim->intersect(ray, info);
}

SurfaceInteraction TransformedPrimitive::setInteractionProperty(
    const HitInfo&         pHit,
    const RayDifferential& ray) const {
  SurfaceInteraction record = prim->setInteractionProperty(pHit, ray);
  return (*primToWorld)(record);
}

Bound3f TransformedPrimitive::worldBounds() const {
  if (prim && primToWorld)
    return (*primToWorld)(prim->worldBounds());
  else
    return Bound3f();
}

std::shared_ptr<TransformedPrimitive> TransformedPrimitive::build(
    const Transform*                  ptw,
    const Transform*                  wtp,
    const std::shared_ptr<Primitive>& prim) {
  return std::make_shared<TransformedPrimitive>(ptw, wtp, prim);
}
}  // namespace Raven