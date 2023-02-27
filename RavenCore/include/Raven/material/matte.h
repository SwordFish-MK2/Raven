#ifndef _RAVEN_MATERIAL_MATTE_H_
#define _RAVEN_MATERIAL_MATTE_H_

#include <Raven/core/base.h>
#include <Raven/core/distribution.h>
#include <Raven/core/material.h>
#include <Raven/core/math.h>
#include <Raven/core/texture.h>
#include <Raven/shading/lambertain.h>
#include <Raven/shading/oren_nayar.h>
namespace Raven {

class MatteMaterial : public Material {
 private:
  std::shared_ptr<Texture<double>>   sigma;  // surface roughness value
  std::shared_ptr<Texture<Spectrum>> kd;     // diffuse reflection value
  std::shared_ptr<Texture<double>>   bump;   // bump function
 public:
  MatteMaterial(const std::shared_ptr<Texture<double>>&   sigma,
                const std::shared_ptr<Texture<Spectrum>>& Kd,
                const std::shared_ptr<Texture<double>>&   bump = NULL)
      : sigma(sigma), kd(Kd), bump(bump) {}

  void computeScarttingFunctions(SurfaceInteraction& its,
                                 bool allowMultipleLobes) const override;

  static Ref<Material> buildConst(double sigma, const Spectrum& kd) {
    Ref<Texture<double>> sigmaTex =
        std::make_shared<ConstTexture<double>>(sigma);
    Ref<Texture<Spectrum>> kdTex = std::make_shared<ConstTexture<Spectrum>>(kd);
    return std::make_shared<MatteMaterial>(sigmaTex, kdTex, nullptr);
  }

  // static Ref<Material> construct(const PropertyList& param) {
  //   const ObjectRef& sigmaRef = param.getObjectRef(0);
  //   const ObjectRef& kdRef    = param.getObjectRef(1);
  //   // const ObjectRef& bump = param.getObjectRef(2);

  //   const Ref<Texture<double>>& sigma =
  //       std::dynamic_pointer_cast<Texture<double>>(sigmaRef.getRef());
  //   const Ref<Texture<Spectrum>>& kd =
  //       std::dynamic_pointer_cast<Texture<Spectrum>>(kdRef.getRef());

  //   return std::make_shared<MatteMaterial>(sigma, kd, nullptr);
  // }
};

// _RAVEN_CLASS_REG_(matte, MatteMaterial, MatteMaterial::construct)
}  // namespace Raven

#endif