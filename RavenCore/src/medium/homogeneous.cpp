#include <Raven/core/interaction.h>
#include <Raven/core/spectrum.h>
#include <Raven/medium/homogeneous.h>

#include <memory>

namespace Raven {
HomogeneousMedium::HomogeneousMedium(const Spectrum& sa, const Spectrum& ss,
    double g)
    : sigma_a(sa), sigma_s(ss), sigma_t(sa + ss), g(g) {}

Spectrum HomogeneousMedium::tr(const Ray& ray, Sampler& sampler) const {
  double d = Min(ray.tMax * ray.dir.length(), MaxFloat);
  return Exp(-sigma_t * d);
}

Spectrum HomogeneousMedium::sample(const Ray& ray, Sampler& sampler,
    Ref<MediumInteraction> minter) const {
  // sample a specific spectra channel
  int channel = Min((int)(sampler.get1D() * Spectrum::sampleNumber),
      Spectrum::sampleNumber - 1);

  // sample the distance of interaction point along the ray
  Float dist = -std::log(1 - sampler.get1D() / sigma_t[channel]);
  Float t = Min(dist * ray.dir.length(), ray.tMax);  // ray distance coefficent

  bool sampleMedium = t < ray.tMax;
  // if the sample point is within the light transport interval, generate a
  // medium interaction
  if (sampleMedium) {
    Ref<PhaseFunction> phase =
        std::make_shared<HenyeyGreensteinPhaseFunction>(g);
        //TODO::SOVLE PROBLEM OF USING SHARED_PTR
    Ref<Medium> medium = std::make_shared<HomogeneousMedium>(sigma_a,sigma_s,g);
    minter = std::make_shared<MediumInteraction>(ray(t), -ray.dir, ray.time, t,
        medium, phase);
  }
  Spectrum Tr      = Exp(-sigma_t * Min(t, MaxFloat) * ray.dir.length());
  Spectrum density = sampleMedium ? (sigma_t * Tr) : Tr;
  Float    pdf     = 0;
  for (int i = 0; i < Spectrum::sampleNumber; i++) pdf += density[i];
  pdf *= 1 / (Float)Spectrum::sampleNumber;
  return sampleMedium ? (Tr * sigma_s / pdf) : (Tr / pdf);
}

}  // namespace Raven