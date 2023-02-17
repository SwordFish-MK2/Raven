#include <Raven/light/areaLight.h>

namespace Raven {
Spectrum DiffuseAreaLight::Li(const Interaction& p, const Vector3f& wi) const {
  return Dot(p.n, wi) > 0.0 ? emittedRadiance : Spectrum(0.0);
  // return emittedRadiance;
}

Spectrum DiffuseAreaLight::sampleLi(const Interaction& inter,
                                    const Point2f&     uv,
                                    LightSample*       lightSample) const {
  // sample a random point on light surface, compute light direction from this
  // point to given point p
  auto [lightInter, pdf] = shape_ptr->sample(inter, uv);
  if (pdf == 0 || DistanceSquared(lightInter.p, inter.p) == 0) {
    return Spectrum(0);
  }

  lightSample->pdf = pdf;
  lightSample->wi  = Normalize(lightInter.p - inter.p);
  // lightSample->n = lightInter.n;
  lightSample->p = lightInter.p;
  return Li(lightInter, -(lightSample->wi));
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

DiffuseAreaLightReg DiffuseAreaLightReg::regHelper;
}  // namespace Raven