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

class Integrator : public RavenObject {
 public:
  Integrator(std::unique_ptr<Camera> c, double epsilon = 0.0001)
      : camera(std::move(c)), epsilon(epsilon) {}

  // 利用传入的Camera渲染场景
  // 渲染的结果输出传入的到Film中，
  // 传入的camera与film为指针以实现多态
  virtual void render(const Scene&) const = 0;

 protected:
  const double            epsilon;
  std::unique_ptr<Camera> camera;
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
                        int                nSample,
                        Sampler&           sampler,
                        bool               handleMedium = false);

Spectrum SampleAllLights(const Interaction& record,
                         const Scene&       scene,
                         Sampler&           sampler,
                         bool               handleMedium = false);
}  // namespace Raven

#endif