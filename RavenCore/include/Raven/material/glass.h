#ifndef _RAVEN_MATERIAL_GLASS_H_
#define _RAVEN_MATERIAL_GLASS_H_

#include <Raven/core/base.h>
#include <Raven/core/material.h>
#include <Raven/shading/specular.h>
#include <Raven/utils/factory.h>

namespace Raven {

class Glass final : public Material {
 public:
  Glass(const std::shared_ptr<Texture<Spectrum>>& kd,
        const std::shared_ptr<Texture<Spectrum>>& kt,
        const std::shared_ptr<Texture<double>>&   uRough,
        const std::shared_ptr<Texture<double>>&   vRough,
        const std::shared_ptr<Texture<double>>&   eta,
        const std::shared_ptr<Texture<double>>&   bump  = nullptr,
        bool                                      remap = true)
      : kdTex(kd),
        ktTex(kt),
        uRough(uRough),
        vRough(vRough),
        bumpTex(bump),
        etaTex(eta),
        remapRoughness(remap) {}

  virtual void computeScarttingFunctions(
      SurfaceInteraction& its,
      bool                allowMultipleLobes) const override;

  // static Ref<Material> construct(const PropertyList& param) {
  //   // TODO::get bump
  //   const ObjectRef& kdRef  = param.getObjectRef(0);
  //   const ObjectRef& ktRef  = param.getObjectRef(1);
  //   const ObjectRef& uRef   = param.getObjectRef(2);
  //   const ObjectRef& vRef   = param.getObjectRef(3);
  //   const ObjectRef& etaRef = param.getObjectRef(4);
  //   bool             remap  = param.getBoolean("remapRoughness", true);
  //   const Ref<Texture<Spectrum>>& kd =
  //       std::dynamic_pointer_cast<Texture<Spectrum>>(kdRef.getRef());
  //   const Ref<Texture<Spectrum>>& kt =
  //       std::dynamic_pointer_cast<Texture<Spectrum>>(ktRef.getRef());
  //   const Ref<Texture<double>>& uRough =
  //       std::dynamic_pointer_cast<Texture<double>>(uRef.getRef());
  //   const Ref<Texture<double>>& vRough =
  //       std::dynamic_pointer_cast<Texture<double>>(vRef.getRef());
  //   const auto& eta =
  //       std::dynamic_pointer_cast<Texture<double>>(etaRef.getRef());
  //   return std::make_shared<Glass>(kd, kt, uRough, vRough, eta, nullptr, remap);
  // }

 private:
  std::shared_ptr<Texture<Spectrum>> kdTex;
  std::shared_ptr<Texture<Spectrum>> ktTex;
  std::shared_ptr<Texture<double>>   uRough;
  std::shared_ptr<Texture<double>>   vRough;
  std::shared_ptr<Texture<double>>   bumpTex;
  std::shared_ptr<Texture<double>>   etaTex;
  bool                               remapRoughness;
};

// _RAVEN_CLASS_REG_(glass, Glass, Glass::construct)
}  // namespace Raven

#endif