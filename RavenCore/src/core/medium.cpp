#include <Raven/core/medium.h>

#include "Raven/core/math.h"

namespace Raven {
double HenyeyGreensteinPhaseFunction::p(const Vector3f &wo,
                                        const Vector3f &wi) const {
  double cosTheta = Dot(wo, wi);
  return PhaseHG(cosTheta, g);
}

std::tuple<Float, Vector3f> HenyeyGreensteinPhaseFunction::sample_p(
    const Vector3f &wo,
    const Point2f   uv) const {
  // compute value of cosTheta
  Float cosTheta = 0;
  if (std::abs(g) < 1e-3)
    cosTheta = 1 - 2 * uv[0];
  else {
    Float sqrTerm = (1 - g * g) / (1 + g - 2 * g * uv[0]);
    cosTheta      = -(1 + g * g - sqrTerm * sqrTerm) / (2 * g);
  }

  // compute value of out going direction
  //
  Float sinTheta = std::sqrt(Max(0.0, 1 - cosTheta * cosTheta));
  Float phi      = 2 * M_PI * uv[1];

  // generate a coordinate system with z axis to wo
  auto [v1, v2] = genTBN(wo);
  Vector3f wi   = SphericalDirection(sinTheta, cosTheta, phi, v1, v2, wo);
  return std::tuple(cosTheta, wi);
}

}  // namespace Raven