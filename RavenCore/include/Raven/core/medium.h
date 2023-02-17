#ifndef _RAVEN_CORE_MEDIA_H_
#define _RAVEN_CORE_MEDIA_H_

#include <Raven/core/base.h>
#include <Raven/core/math.h>
#include <Raven/core/object.h>
#include <Raven/core/sampler.h>
#include <Raven/core/spectrum.h>

#include <tuple>

namespace Raven {
/// <summary>
/// 介质边界的接口，包含表面两侧介质的指针
///</summary>
struct MediumInterface {
 public:
  // interface
  MediumInterface(const std::shared_ptr<Medium>& medium)
      : outside(medium), inside(medium) {}

  MediumInterface(const std::shared_ptr<Medium>& outside,
                  const std::shared_ptr<Medium>& inside)
      : outside(outside), inside(inside) {}

  MediumInterface() : outside(nullptr), inside(nullptr) {}

  bool isMediumTransition() const { return inside != outside; }
  // data
  Ref<Medium> outside;
  Ref<Medium> inside;
};

/// <summary>
/// PhaseFunction接口
/// </summary>
class PhaseFunction {
 public:
  // 计算Phase Function 的值
  virtual Float p(const Vector3f& wo, const Vector3f& wi) const = 0;

  virtual std::tuple<Float, Vector3f> sample_p(const Vector3f& wo,
                                               const Point2f   uv) const = 0;
};

/// <summary>
/// 由Henyey Greenstein提出的各项同性phase funcion模型
/// </summary>
class HenyeyGreensteinPhaseFunction final : public PhaseFunction {
 public:
  HenyeyGreensteinPhaseFunction(Float g) : g(g) {}

  double p(const Vector3f& wo, const Vector3f& wi) const override;

  std::tuple<Float, Vector3f> sample_p(const Vector3f& wo,
                                       const Point2f   uv) const override;

 private:
  double g;
};

/// <summary>
/// 介质接口
/// </summary>
class Medium {
 public:
  // 假设传入该函数的光线在传播过程中未被遮挡并且完全包围在当前介质中，计算光线的beam
  // transmittance
  virtual Spectrum tr(const Ray& ray, Sampler& sampler) const = 0;

  // sample a intersection point alone the ray,campute the ratio of the light
  // transmittence and sampling pdf
  virtual Spectrum sample(const Ray&             ray,
                          Sampler&               sampler,
                          Ref<MediumInteraction> minter) const = 0;
};

inline double PhaseHG(double cosTheta, double g) {
  double inv4PI = 0.25 * M_PI;
  double denom  = 1 + g * g + 2 * g * (cosTheta);
  return inv4PI * (1 - g * g) / denom * sqrt(denom);
}

}  // namespace Raven
#endif