#ifndef _RAVEN_INTEGRATOR_VOLUME_PATHTRACING__H_
#define _RAVEN_INTEGRATOR_VOLUME_PATHTRACING__H_

#include <Raven/core/integrator.h>

namespace Raven {

class VolumePathTracingIntegrator final : public Integrator {
 public:
  VolumePathTracingIntegrator(std::unique_ptr<Camera>  c,
                              std::unique_ptr<Sampler> s,
                              LightSampleStrategy      strategy,
                              int                      maxDepth = 64,
                              double                   epslion  = 1e-6)
      : Integrator(std::move(c), std::move(s), strategy, epslion),
        maxDepth(maxDepth) {}

  Spectrum renderPixel(const Scene&           scene,
                       const RayDifferential& ray,
                       Sampler*               sampler) const override;

 private:
  int              maxDepth;
  std::vector<int> nLightSamples;
};

}  // namespace Raven

#endif