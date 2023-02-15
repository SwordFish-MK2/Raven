#ifndef _RAVEN_MEDIUM_HOMOGENEOUS_H_
#define _RAVEN_MEDIUM_HOMOGENEOUS_H_

#include <Raven/core/base.h>
#include <Raven/core/medium.h>
#include <Raven/core/ray.h>
namespace Raven {
class HomogeneousMedium : public Medium {
 public:
  HomogeneousMedium(const Spectrum& sa, const Spectrum& ss, double g);

  virtual Spectrum tr(const Ray& ray, Sampler& sampler) const override;

  virtual Spectrum sample(const Ray& ray, Sampler& sampler,
      Ref<MediumInteraction> minter) const override;

 private:
  const Spectrum sigma_a;  // absorb
  const Spectrum sigma_s;  // scatter
  const Spectrum sigma_t;  // transmittence
  const double   g;        // parameter of phase function
};
}  // namespace Raven

#endif