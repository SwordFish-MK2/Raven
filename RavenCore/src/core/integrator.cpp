#include <Raven/core/integrator.h>
#include <Raven/core/light.h>

#include "Raven/core/base.h"

namespace Raven {

Spectrum EvaluateLight(const Interaction& record,
                       const Scene&       scene,
                       const Light&       light) {
  // 处理光源
  for (std::shared_ptr<Light> light : scene.lights) {
    light->preprocess(scene);
  }

  bool     isSurface = record.isSurfaceInteraction();
  Spectrum L(0.0);

  // sample from distribution of light
  LightSample lSample;
  Spectrum Le = light.sampleLi(record, Point2f(GetRand(), GetRand()), &lSample);

  // handle the case that the shadow ray sampled is occluded
  if (!Le.isBlack()) {
    Ray shadowRay = record.scartterRay(lSample.wi);  // shadow ray从交点射向光源
    double distance = (record.p - lSample.p).length();
    shadowRay.tMax  = distance - 0.01;
    if (scene.hit(shadowRay))
      Le = Spectrum(0.0);  // 如果shadow ray被遮挡，返回Radiance=0

    if (Le != Spectrum(0.0) && lSample.pdf > 0) {
      Spectrum f{0.0};
      Float    scarttingPdf = 0.0;

      // compute f and pdf
      if (isSurface) {
        // current interaction happened on object surface,compute bsdf value and
        // corresponding pdf
        const SurfaceInteraction& si = (const SurfaceInteraction&)record;
        si.bsdf->f(record.wo, lSample.wi) * std::abs(Dot(lSample.wi, record.n));
        scarttingPdf = si.bsdf->pdf(record.wo, lSample.wi);
      } else {
        // current interaction happened inside medium volume, compute phase
        // function value and pdf
        const MediumInteraction& mi = (const MediumInteraction&)record;

        Float phase  = mi.phase->p(mi.wo, lSample.wi);
        f            = Spectrum(phase);
        scarttingPdf = phase;
      }

      // compute the weight of sampling
      if (!f.isBlack() && scarttingPdf != 0.0) {
        double weight = PowerHeuristic(1, lSample.pdf, 1, scarttingPdf);
        L             += Le * f * weight / lSample.pdf;
      }
    }
  }

  // sampling from distribution of brdf

  // sample brdf
  if (!light.isDeltaLight()) {
    Spectrum f;
    Float    scarttingPdf;
    Vector3f wi;
    if (isSurface) {
      const SurfaceInteraction& si = (const SurfaceInteraction&)record;
      auto [fValue, wiSurface, pdf, sampledType] =
          si.bsdf->sample_f(record.wo, Point2f(GetRand(), GetRand()));
      f            = fValue;
      wi           = wiSurface;
      scarttingPdf = pdf;
    } else {
      const MediumInteraction& mi = (const MediumInteraction&)record;
      auto [phase, wiMedium] =
          mi.phase->sample_p(mi.wo, Point2f(GetRand(), GetRand()));
      f            = Spectrum(phase);
      wi           = wiMedium;
      scarttingPdf = phase;
    }

    double lightPdf = 1.0;
    if (f != Spectrum(0.0) && scarttingPdf > 0) {
      double cosTheta = abs(Dot(wi, record.n));
      f               *= cosTheta;

      // 计算lightPdf，求MIS权重
      lightPdf = light.pdf_Li(record, wi);
      if (lightPdf == 0)
        return L;
      double weight = PowerHeuristic(1, scarttingPdf, 1, lightPdf);

      // 出射shadow ray 判断光源是否被遮挡
      Ray                               shadowRay = record.scartterRay(wi);
      std::optional<SurfaceInteraction> intersection =
          scene.intersect(shadowRay);

      // 采样的shadow ray未被遮挡，计算光源的贡献
      Spectrum Le(0.0);
      if (intersection) {
        if (intersection->hitLight && intersection->light == &light)
          Le += intersection->Le(-wi);
      } else
        Le += light.Le(shadowRay);
      if (!Le.isBlack())
        L += weight * f * Le / scarttingPdf;
    }
  }
  return L;
}

// 在每个光源上采样一个点，计算Radiance
Spectrum SampleAllLights(const Interaction& record, const Scene& scene) {
  Spectrum Le(0.0);
  for (auto light : scene.lights) {
    Le += EvaluateLight(record, scene, *light.get());
  }
  return Le;
}

Spectrum SampleOneLight(const Interaction& record,
                        const Scene&       scene,
                        int                nSample) {
  Spectrum                                   Le(0.0);
  const std::vector<std::shared_ptr<Light>>& lights = scene.lights;
  Spectrum                                   totalPower(0.0);
  for (auto light : lights) { totalPower += light->power(); }
  Spectrum power;
  double   pr = GetRand();
  double   p  = 1.0;
  Light*   chosen;
  for (size_t i = 0; i < lights.size(); i++) {
    power += lights[i]->power();
    p     = power.y() / totalPower.y();
    if (p >= pr || i == lights.size() - 1) {
      chosen = lights[i].get();
      break;
    }
  }

  for (int i = 0; i < nSample; i++)
    Le += EvaluateLight(record, scene, *chosen) / p;
  return Le / nSample;
}
}  // namespace Raven