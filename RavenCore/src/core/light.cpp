#include <Raven/core/light.h>
#include <Raven/core/scene.h>
#include <Raven/core/spectrum.h>


namespace Raven {

void LightSample::testVisibility(const Interaction &inter, const Scene &scene) {
  Ray    shadowRay = inter.scartterRay(wi);
  double distance  = (inter.p - p).length();
  shadowRay.tMax   = distance - 0.01;
  if (scene.hit(shadowRay))
    le = Spectrum(0.0);
}

void LightSample::handleMedium(const Interaction &inter,
                               const Scene       &scene,
                               Sampler           &sampler) {
  Ray    shadowRay = inter.scartterRay(wi);
  double distance  = (inter.p - p).length();
  shadowRay.tMax   = distance - 0.01;
  while (true) {
    auto si = scene.intersect(shadowRay);
    if (si.has_value() && si->mate_ptr != nullptr)
      le = Spectrum(0.0);
    if (shadowRay.medium)
      le *= shadowRay.medium->tr(shadowRay, sampler);
    if (!si.has_value())
      break;
    shadowRay = si->scartterRay(wi);
  }
}
}  // namespace Raven