#ifndef _RAVEN_RENDERER_PATH_TRACING_H_
#define _RAVEN_RENDERER_PATH_TRACING_H_

#include <Raven/core/base.h>
#include <Raven/core/integrator.h>
#include <Raven/core/spectrum.h>
#include <Raven/utils/propertylist.h>

namespace Raven {
class PathTracingIntegrator final : public Integrator {
 public:
  PathTracingIntegrator(std::unique_ptr<Camera>  c,
                        std::unique_ptr<Sampler> s,
                        int                      maxDepth = 64,
                        double                   epslion  = 1e-6)
      : Integrator(std::move(c), epslion),
        sampler(std::move(s)),
        maxDepth((maxDepth)) {}
  void render(const Scene&) const override;

  // static Ref<PathTracingIntegrator> construct(const PropertyList& param);

 private:
  Spectrum integrate(const Scene&           scene,
                     const RayDifferential& r_in,
                     int                    depth = 0) const;
  //	GeometryData gBuffer(const Ray& ray, const Scene& scene)const;

 private:
  const int maxDepth;

  std::unique_ptr<Sampler> sampler;
};

// _RAVEN_CLASS_REG_(path, PathTracingIntegrator, PathTracingIntegrator::construct)
}  // namespace Raven

#endif