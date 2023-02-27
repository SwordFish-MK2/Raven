#ifndef _RAVEN_CORE_RENDERER_H_
#define _RAVEN_CORE_RENDERER_H_

#include <Raven/core/base.h>
#include <Raven/core/bsdf.h>
#include <Raven/core/camera.h>
#include <Raven/core/film.h>
#include <Raven/core/interaction.h>
#include <Raven/core/object.h>
#include <Raven/core/scene.h>
#include <Raven/core/spectrum.h>

#include <memory>

namespace Raven {

enum class LightSampleStrategy {
  UniformlySampleAllLights,
  UniformlySampleOneLight
};

class Integrator : public RavenObject {
 public:
  Integrator(std::unique_ptr<Camera>  c,
             std::unique_ptr<Sampler> s,
             LightSampleStrategy      strategy,
             double                   epsilon = 0.0001)
      : camera(std::move(c)),
        sampler(std::move(s)),
        strategy(strategy),
        epsilon(epsilon) {}

  // 利用传入的Camera渲染场景
  // 渲染的结果输出传入的到Film中，
  // 传入的camera与film为指针以实现多态
  virtual void render(const Scene& scene, int threadNumber = 0) const;

  virtual Spectrum renderPixel(const Scene&           scene,
                               const RayDifferential& ray,
                               Sampler*               sampler) const = 0;

  virtual void preProcess(const Scene& scene) {}

 protected:
  const double            epsilon;
  std::unique_ptr<Camera> camera;
  LightSampleStrategy     strategy;
  std::unique_ptr<Sampler> sampler;
};

// 计算MIS weight
inline double PowerHeuristic(int fNum, double fPdf, int gNum, double gPdf) {
  double f = fNum * fPdf;
  double g = gNum * gPdf;
  return f * f / (f * f + g * g);
}

Spectrum EvaluateLight(const Interaction& record,
                       const Scene&       scene,
                       const Light&       light,
                       Sampler&           sampler,
                       bool               handleMedium = false);

Spectrum SampleOneLight(const Interaction& record,
                        const Scene&       scene,
                        Sampler&           sampler,
                        bool               handleMedium = false);

Spectrum SampleAllLights(const Interaction&      record,
                         const Scene&            scene,
                         Sampler&                sampler,
                         const std::vector<int>& nLightSamples,
                         bool                    handleMedium);
}  // namespace Raven

#endif