#include <Raven/core/integrator.h>
#include <Raven/core/interaction.h>
#include <Raven/core/light.h>
#include <Raven/core/medium.h>
#include <Raven/integrator/volpath.h>

#include <memory>

namespace Raven {

Spectrum VolumePathTracingIntegrator::renderPixel(
    const Scene           &scene,
    const RayDifferential &r_in,
    Sampler               *threadSampler) const {
  Spectrum        Li{0.0};    // radiance of current sample
  Spectrum        beta{1.0};  // attenuation coefficent
  RayDifferential ray = r_in;

  bool  specularBounce = false;
  Float etaScale       = 1;
  for (int bounce = 0; bounce < maxDepth; bounce++) {
    // find closet surface intersection
    std::optional<SurfaceInteraction> record = scene.intersect(ray);

    // handle ray-media intersection
    std::shared_ptr<MediumInteraction> mi;
    if (ray.medium)
      beta *= ray.medium->sample(ray, *sampler, mi);
    if (beta.isBlack())
      break;

    // account for medium intersection or surface intersection according to
    // whether medium interaction is found

    // found medium intersection
    if (mi->isValid()) {
      Spectrum emit    = SampleOneLight(*mi, scene, *sampler, true);
      Li               += beta * emit;
      Vector3f wo      = -ray.dir;
      auto [phase, wi] = mi->phase->sample_p(wo, sampler->get2D());
      ray              = mi->scatterRay(wi);
    }

    // not found medium intersection
    else {
      // if the ray started from camera hits light source directly
      if (bounce == 0 || specularBounce) {
        // handle cases that ray hits area lights directly
        if (record.has_value())
          Li += beta * record->Le(-ray.dir);

        // account directional light and environment light
        else
          for (const auto &light : scene.lights) Li += beta * light->Le(ray);
      }

      // sample lights
      if (strategy == LightSampleStrategy::UniformlySampleAllLights) {
        Spectrum L_dir = SampleAllLights(*record, scene, *(threadSampler),
                                         nLightSamples, false);
        Li             += beta * L_dir;
      } else {
        Spectrum L_dir =
            SampleOneLight(*record, scene, *(threadSampler), false);
        Li += beta * L_dir;
      }

      // get intesection information
      Point3f  p  = record->p;
      Vector3f wo = Normalize(-ray.dir);
      Normal3f n  = record->n;

      // sample brdf and ray scatting direction
      auto [f, wi, pdf, sampledType] =
          record->bsdf->sample_f(wo, threadSampler->get2D());
      if (f == Spectrum(0.0) || pdf == 0.0)
        break;

      // update beta
      double cosTheta = abs(Dot(wi, n));
      beta            *= f * cosTheta / pdf;
      specularBounce  = (sampledType & BxDFType::Specular) != 0;
      ray             = record->scatterRay(wi);
    }

    // perform russian roulette
    if (bounce > 3) {
      double q = Max((double).05, 1 - beta.y());
      if (threadSampler->get1D() < q)
        break;
      beta /= 1 - q;
    }
  }
  return Li;
}
}  // namespace Raven