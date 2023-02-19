#include <Raven/core/base.h>
#include <Raven/core/integrator.h>
#include <Raven/core/light.h>

#include "Raven/core/bxdf.h"

namespace Raven {

Spectrum EvaluateLight(const Interaction& record,
                       const Scene&       scene,
                       const Light&       light,
                       Sampler&           sampler,
                       bool               handleMedium) {
  // 处理光源
  for (std::shared_ptr<Light> light : scene.lights) {
    light->preprocess(scene);
  }

  bool     isSurface = record.isSurfaceInteraction();
  Spectrum f{0.0};
  Float    scatteringPdf{0.0};
  //                                                                                                                                                                      totle light radiance
  Spectrum L(0.0);

  // take a sample from light sampling distribution
  auto lSample = light.sampleLi(record, Point2f(GetRand(), GetRand()));

  if (lSample.has_value()) {
    if (handleMedium) {
      lSample->handleMedium(record, scene, sampler);
    } else {
      lSample->testVisibility(record, scene);
    }

    // compute BSDF or phase function value
    if (lSample->le != Spectrum(0.0) && lSample->pdf > 0) {
      // surfaceInteraction, compute bsdf value
      if (isSurface) {
        const SurfaceInteraction& si = (const SurfaceInteraction&)record;

        f = si.bsdf->f(record.wo, lSample->wi) *
            std::abs(Dot(lSample->wi, record.n));
        scatteringPdf = si.bsdf->pdf(record.wo, lSample->wi);
      }

      // mediumInteraction, compute phase function value
      else {
        const MediumInteraction& mi = (const MediumInteraction&)record;

        Float phase   = mi.phase->p(mi.wo, lSample->wi);
        f             = Spectrum(phase);
        scatteringPdf = phase;
      }

      // compute the weight of sampling
      if (!f.isBlack() && scatteringPdf != 0.0) {
        if (light.isDeltaLight())
          L += lSample->le * f / lSample->pdf;
        else {
          double weight = PowerHeuristic(1, lSample->pdf, 1, scatteringPdf);
          L             += lSample->le * f * weight / lSample->pdf;
        }
      }
    }
  }

  // take a sample from brdf sampling distribution

  // sample brdf
  if (!light.isDeltaLight()) {
    Vector3f wi{0.0};
    bool     sampledSpecular = false;

    // sample bsdf or phase function
    if (isSurface) {
      const SurfaceInteraction& si = (const SurfaceInteraction&)record;
      auto [fValue, wiSurface, pdf, sampledType] =
          si.bsdf->sample_f(record.wo, Point2f(GetRand(), GetRand()));
      double cosTheta = std::abs(Dot(wiSurface, si.shading.n));
      f               = fValue * cosTheta;
      wi              = wiSurface;
      scatteringPdf   = pdf;
      sampledSpecular = sampledType & BxDFType::Specular;
    } else {
      const MediumInteraction& mi = (const MediumInteraction&)record;
      auto [phase, wiMedium] =
          mi.phase->sample_p(mi.wo, Point2f(GetRand(), GetRand()));
      f             = Spectrum(phase);
      wi            = wiMedium;
      scatteringPdf = phase;
    }

    // compute pdf from light distribution of current sampled direction
    if (!f.isBlack() && scatteringPdf > 0) {
      Float lightPdf = 1.0;
      Float weight   = 1.0;

      // only perform mis if the sampled bsdf is non-specular
      if (!sampledSpecular) {
        lightPdf = light.pdf_Li(record, wi);

        // shadow ray hits no light, return
        if (lightPdf == 0)
          return L;

        // sample ray hits light, perform mis
        weight = PowerHeuristic(1, scatteringPdf, 1, lightPdf);
      }
      Ray  shadowRay = record.scartterRay(wi);
      auto lightSi   = handleMedium ? scene.intersect(shadowRay)
                                    : scene.tr(shadowRay, sampler);

      Spectrum Le(0.0);
      if (lightSi.has_value()) {
        // shadow ray hits sampled area light, else light emittion is blocked
        if (lightSi->hitLight && lightSi->light == &light)
          Le = lightSi->Le(-wi);
      } else
        // shadow ray hits infinite light
        Le = light.Le(shadowRay);
      if (!Le.isBlack())
        L += weight * f * Le / scatteringPdf;
    }
  }
  return L;
}

// 在每个光源上采样一个点，计算Radiance
Spectrum SampleAllLights(const Interaction& record,
                         const Scene&       scene,
                         Sampler&           sampler,
                         bool               handleMedium) {
  Spectrum Le(0.0);
  for (auto light : scene.lights) {
    Le += EvaluateLight(record, scene, *light.get(), sampler, handleMedium);
  }
  return Le;
}

Spectrum SampleOneLight(const Interaction& record,
                        const Scene&       scene,
                        int                nSample,
                        Sampler&           sampler,
                        bool               handleMedium) {
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
    Le += EvaluateLight(record, scene, *chosen, sampler, handleMedium) / p;
  return Le / nSample;
}
}  // namespace Raven