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
    // find closet surface interaction
    std::optional<SurfaceInteraction> record = scene.intersect(ray);

    // account paticipating media
    std::shared_ptr<MediumInteraction> mi;
    if (ray.medium)
      beta *= ray.medium->sample(ray, *sampler, mi);
    if (beta.isBlack())
      break;

    // whether medium interaction is found
    if (mi->isValid()) {
      Spectrum emit    = SampleOneLight(*mi, scene, *sampler, true);
      Li               += beta * emit;
      Vector3f wo      = -ray.dir;
      auto [phase, wi] = mi->phase->sample_p(wo, sampler->get2D());
      ray              = mi->scatterRay(wi);
    } else {
      // 只有从相机出发的光线击中光源才直接返回光源的emittion
      if (bounce == 0 || specularBounce) {
        // handle cases that ray hits lights directly
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

      // 采样brdf，计算出射方向,更新beta
      auto [f, wi, pdf, sampledType] =
          record->bsdf->sample_f(wo, threadSampler->get2D());
      if (f == Spectrum(0.0) || pdf == 0.0)
        break;

      // 计算衰减
      double cosTheta = abs(Dot(wi, n));
      beta            *= f * cosTheta / pdf;
      // std::cout <<f<< beta << std::endl;
      specularBounce = (sampledType & BxDFType::Specular) != 0;
      ray            = record->scatterRay(wi);
    }

    // 俄罗斯轮盘赌结束循环
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