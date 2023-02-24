#include <Raven/core/base.h>
#include <Raven/core/bxdf.h>
#include <Raven/core/integrator.h>
#include <Raven/core/light.h>

#include <memory>

namespace Raven {

Spectrum EvaluateLight(const Interaction& record,
                       const Scene&       scene,
                       const Light&       light,
                       const Point2f&     uLight,
                       const Point2f&     uScatter,
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
  auto lSample = light.sampleLi(record, uLight);

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
          si.bsdf->sample_f(record.wo, uScatter);
      double cosTheta = std::abs(Dot(wiSurface, si.shading.n));
      f               = fValue * cosTheta;
      wi              = wiSurface;
      scatteringPdf   = pdf;
      sampledSpecular = sampledType & BxDFType::Specular;
    } else {
      const MediumInteraction& mi = (const MediumInteraction&)record;
      auto [phase, wiMedium]      = mi.phase->sample_p(mi.wo, uScatter);
      f                           = Spectrum(phase);
      wi                          = wiMedium;
      scatteringPdf               = phase;
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
      Ray  shadowRay = record.scatterRay(wi);
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
Spectrum SampleAllLights(const Interaction&      record,
                         const Scene&            scene,
                         Sampler&                sampler,
                         const std::vector<int>& nLightSamples,
                         bool                    handleMedium) {
  Spectrum Le(0.0);

  for (int i = 0; i < scene.lights.size(); i++) {
    const std::shared_ptr<Light>& light = scene.lights[i];

    // retrive sample arrays from sampler
    int            nSamples       = nLightSamples[i];
    const Point2f* uLightArray    = sampler.get2DArray(nSamples);
    const Point2f* uScartterArray = sampler.get2DArray(nSamples);

    // all prerequested sample arrays is consumed, use single sample instead
    if (!uLightArray || !uScartterArray) {
      Point2f uLight   = sampler.get2D();
      Point2f uScatter = sampler.get2D();
      Le += EvaluateLight(record, scene, *light.get(), uLight, uScatter,
                          sampler, handleMedium);
    }

    // use sample arrays to estimate light
    else {
      Spectrum L(0.);
      for (int j = 0; j < nSamples; j++)
        L += EvaluateLight(record, scene, *light.get(), uLightArray[j],
                           uScartterArray[j], sampler, handleMedium);
      Le += L / nSamples;
    }
  }
  return Le;
}

Spectrum SampleOneLight(const Interaction& record,
                        const Scene&       scene,
                        Sampler&           sampler,
                        bool               handleMedium) {
  const std::vector<std::shared_ptr<Light>>& lights = scene.lights;

  Spectrum Le(0.0);

  if (lights.size() == 0)
    return Spectrum(0.0);

  // randomly choose a light to sample at a posibilty proportional to its
  // emitted power
  Spectrum totalPower(0.0);
  for (auto light : lights) { totalPower += light->power(); }
  Spectrum accumulatedPower{0.0};
  double   pr  = sampler.get1D();
  double   p   = 1.0;
  Float    pdf = 1.0;
  Light*   chosen;
  for (size_t i = 0; i < lights.size(); i++) {
    Spectrum power   = lights[i]->power();
    accumulatedPower += power;
    p                = accumulatedPower.y() / totalPower.y();
    pdf              = power.y() / totalPower.y();
    if (p >= pr || i == lights.size() - 1) {
      chosen = lights[i].get();
      break;
    }
  }

  // sample chosen light
  Point2f uLight   = sampler.get2D();
  Point2f uScatter = sampler.get2D();

  Le        += EvaluateLight(record, scene, *chosen, uLight, uScatter, sampler,
                             handleMedium);
  return Le /= pdf;
}
}  // namespace Raven