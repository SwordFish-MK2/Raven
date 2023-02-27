#include <Raven/light/areaLight.h>

#include "Raven/core/light.h"

namespace Raven {
Spectrum DiffuseAreaLight::Li(const Interaction& p, const Vector3f& wi) const {
  return Dot(p.n, wi) > 0.0 ? emittedRadiance : Spectrum(0.0);
  // return emittedRadiance;
}

std::optional<LightSample> DiffuseAreaLight::sampleLi(const Interaction& inter,
                                                      const Point2f& uv) const {
  // sample a random point on light surface, compute light direction from this
  // point to given point p
  auto [lightInter, pdf] = shape_ptr->sample(inter, uv);

  if (pdf == 0 || DistanceSquared(lightInter.p, inter.p) == 0) {
    return std::nullopt;
  }

  Vector3f wi = Normalize(lightInter.p - inter.p);
  Spectrum le = Li(lightInter, -wi);
  if (le.isBlack())
    return std::nullopt;
  return LightSample{le, lightInter.p, wi, pdf};
}

// Vector3f DiffuseAreaLight::sample_Li(const SurfaceInteraction& inter, const
// Point2f& uv, 	Vector3f* wi, double* pdf, SurfaceInteraction*
// lightSample)const
//{
//	//在光源表面采样一个点，算出从该点射向点p的方向向量
//	SurfaceInteraction lightInter = shape_ptr->sample(inter, uv);
//	*wi = Normalize(lightInter.p - inter.p);
//	//调用Shape的pdf函数求出采样该点的pdf
//	*pdf = shape_ptr->pdf(lightInter);
//	*lightSample = lightInter;
//	return	Li(lightInter, -*wi);
// }

Spectrum DiffuseAreaLight::power() const {
  return emittedRadiance * area * M_PI;
}

double DiffuseAreaLight::pdf_Li(const Interaction& inter,
                                const Vector3f&    wi) const {
  return shape_ptr->pdf(inter, wi);
}

// DiffuseAreaLightReg DiffuseAreaLightReg::regHelper;
}  // namespace Raven