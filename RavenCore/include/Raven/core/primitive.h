#ifndef _RAVEN_CORE_PRIMITIVE_H_
#define _RAVEN_CORE_PRIMITIVE_H_

#include <Raven/core/base.h>
#include <Raven/core/material.h>
#include <Raven/core/object.h>
#include <Raven/core/shape.h>

#include "Raven/core/interaction.h"
#include "Raven/core/ray.h"

namespace Raven {

// primitive inter
class Primitive : public RavenObject {
public:
  // whether the incident ray hits the geometric shape of current primitive
  virtual bool hit(const Ray& r_in) const = 0;

  // check if incident ray hit current primitive and record hitInfo of
  //  intersection, if any
  virtual bool intersect(const Ray& r_in, HitInfo& inter) const = 0;

  // given hitInfo, compute surfaceInteraction
  virtual SurfaceInteraction setInteractionProperty(
      const HitInfo&         p,
      const RayDifferential& ray) const = 0;

  virtual Bound3f worldBounds() const = 0;
};

//
class GeometryPrimitive final : public Primitive {
public:
  GeometryPrimitive(const std::shared_ptr<Shape>&     shape_ptr,
                    const std::shared_ptr<Material>&  mate_ptr,
                    const std::shared_ptr<AreaLight>& light=nullptr,
                    const MediumInterface&            medium=MediumInterface{})
      : shape_ptr(shape_ptr),
        mate_ptr(mate_ptr),
        light_ptr(light),
        mediumInterface(medium) {}
  bool hit(const Ray& ray) const override;

  bool intersect(const Ray& ray, HitInfo& inter) const override;

  SurfaceInteraction setInteractionProperty(
      const HitInfo&         p,
      const RayDifferential& ray) const override;

  Bound3f worldBounds() const override;

  const AreaLight* getAreaLight() const { return light_ptr.get(); }

  // static Ref<GeometryPrimitive> build(const Ref<PropertyList>& param);

private:
  std::shared_ptr<Shape>     shape_ptr;
  std::shared_ptr<Material>  mate_ptr;
  std::shared_ptr<AreaLight> light_ptr;
  MediumInterface            mediumInterface;
};

class TransformedPrimitive final : public Primitive {
public:
  TransformedPrimitive(const Transform*                 ptw,
                       const Transform*                 wtp,
                       const std::shared_ptr<Primitive> p)
      : primToWorld(ptw), worldToPrim(wtp), prim(p) {}

  bool hit(const Ray& r_in) const override;

  bool intersect(const Ray& ray, HitInfo& info) const override;

  SurfaceInteraction setInteractionProperty(
      const HitInfo&         p,
      const RayDifferential& ray) const override;

  Bound3f worldBounds() const override;

  static std::shared_ptr<TransformedPrimitive> build(
      const Transform*                  ptw,
      const Transform*                  wtp,
      const std::shared_ptr<Primitive>& prim);

private:
  const Transform*                 primToWorld;
  const Transform*                 worldToPrim;
  const std::shared_ptr<Primitive> prim;
};


}  // namespace Raven

#endif