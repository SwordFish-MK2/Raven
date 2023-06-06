#ifndef _RAVEN_MATERIAL_PLASTIC_H_
#define _RAVEN_MATERIAL_PLASTIC_H_

#include <Raven/core/base.h>
#include <Raven/core/material.h>
#include <Raven/core/texture.h>
#include <Raven/shading/microfacet.h>

namespace Raven {
class Plastic : public Material {
 public:
  Plastic(const std::shared_ptr<Texture<Spectrum>>& kdTexture,
          const std::shared_ptr<Texture<Spectrum>>& ksTexture,
          const std::shared_ptr<Texture<double>>&   roughnessTexture,
          const std::shared_ptr<Texture<double>>&   bumpTexture = nullptr)
      : kd(kdTexture),
        ks(ksTexture),
        roughness(roughnessTexture),
        bump(bumpTexture) {}

  void computeScarttingFunctions(SurfaceInteraction& its,
                                 bool allowMultipleLobes) const override;

  // static Ref<Material> construct(const PropertyList& param) {
  //   const auto& kd = std::dynamic_pointer_cast<Texture<Spectrum>>(
  //       param.getObjectRef(0).getRef());
  //   const auto& ks = std::dynamic_pointer_cast<Texture<Spectrum>>(
  //       param.getObjectRef(1).getRef());
  //   // const auto& roughness =
  //   // std::dynamic_pointer_cast<Texture<Spectrum>>(param.getObjectRef(2).getRef());

  //   return std::make_shared<Plastic>(kd, ks, nullptr);
  // }

 private:
  std::shared_ptr<Texture<Spectrum>> kd;
  std::shared_ptr<Texture<Spectrum>> ks;
  std::shared_ptr<Texture<double>>   roughness;
  std::shared_ptr<Texture<double>>   bump;
};

// _RAVEN_CLASS_REG_(plastic, Plastic, Plastic::construct)
}  // namespace Raven

#endif