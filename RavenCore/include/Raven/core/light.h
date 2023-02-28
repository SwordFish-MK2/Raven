#ifndef _RAVEN_CORE_LIGHT_H_
#define _RAVEN_CORE_LIGHT_H_

#include <Raven/core/base.h>
#include <Raven/core/interaction.h>
#include <Raven/core/math.h>
#include <Raven/core/object.h>
#include <Raven/core/sampler.h>
#include <Raven/core/shape.h>
#include <Raven/core/spectrum.h>

namespace Raven {
enum class LightFlag : int {
  DeltaPosition  = 1,
  DeltaDirection = 2,
  AreaLight      = 4,
  InfiniteLight  = 8
};

struct LightSample {
  Spectrum le;
  Point3f  p;
  Vector3f wi;
  Float    pdf;

  LightSample(const Spectrum& le, const Point3f p, const Vector3f wi, Float pdf)
      : le(le), p(p), wi(wi), pdf(pdf) {}

  // test if the shadow ray sampled is occluded by objects in the scene
  void testVisibility(const Interaction& inter, const Scene& scene);

  void handleMedium(const Interaction& inter,
                    const Scene&       scene,
                    Sampler&           sampler);
};

/// <summary>
/// 光源类接口，所有光源必须继承该类
/// </summary>
class Light : public RavenObject {
 public:
  Light(const Ref<Transform>& LTW,
        const Ref<Transform>& WTL,
        int                   flag,
        int                   nSamples)
      : lightToWorld(LTW), worldToLight(WTL), flag(flag), nSamples(nSamples) {}

  // return radiance reached the given point emitted by light source, compute
  // light incident direction and sampling pdf
  virtual std::optional<LightSample> sampleLi(const Interaction& inter,
                                              const Point2f&     uv) const = 0;

  // return total power emitted by light source
  virtual Spectrum power() const = 0;

  virtual Spectrum Le(const RayDifferential& ray) const {
    return Spectrum(0.0);
  }

  virtual double pdf_Li(const Interaction& inter, const Vector3f& wi) const = 0;

  virtual void preprocess(const Scene& scene) {}

  virtual bool isDeltaLight() const {
    return flag & (int)LightFlag::DeltaPosition ||
           flag & (int)LightFlag::DeltaDirection;
  }

 public:
  const int nSamples;  // n shadow rays prefered by the integrater

 protected:
  const int            flag;  // whether light contines delta distribution
  const Ref<Transform> lightToWorld;
  const Ref<Transform> worldToLight;
};


}  // namespace Raven

#endif