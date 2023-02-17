#include <Raven/shading/specular.h>

namespace Raven {

Spectrum SpecularReflection::sampled_f(const Vector3f& wo,
                                       Vector3f&       wi,
                                       const Point2f&  sample,
                                       double&         pdf) const {
  wi  = Vector3f(-wo.x, -wo.y, wo.z);
  pdf = 1.0;
  return fresnel->evaluate(CosTheta(wi)) * r / AbsCosTheta(wi);
}

Spectrum SpecularTransmission::sampled_f(const Vector3f& wo,
                                         Vector3f&       wi,
                                         const Point2f&  sample,
                                         double&         pdf) const {
  // 判断光线为入射还是出射
  bool enter = wo.z > 0;

  // 如果光线为出射，交换介质两边的折射率与法线
  double   etai = enter ? etaA : etaB;
  double   etat = enter ? etaB : etaA;
  double   eta  = etai / etat;
  Normal3f n    = enter ? Normal3f(0, 0, 1) : Normal3f(0, 0, -1);

  auto wt = Refract(wo, n, etai / etat);
  if (!wt.has_value())
    return Spectrum(0.0);

  wi  = *wt;
  pdf = 1.0;
  Spectrum tau =
      t * (1 - fresnel.evaluate(CosTheta(*wt)));  // 折射进入介质的能量

  return eta * eta * tau / AbsCosTheta(*wt);
}

Spectrum FresnelSpecular::sampled_f(const Vector3f& wo,
                                    Vector3f&       wi,
                                    const Point2f&  sample,
                                    double&         pdf) const {
  double F       = FDielectric(CosTheta(wo), etaA, etaB);
  bool   reflect = F > sample[0];
  if (reflect) {
    wi  = Reflect(wo);
    pdf = F;
    return F * R / AbsCosTheta(wi);  //?
  } else {
    Spectrum ft = (1 - F) * T;

    // 判断光线为入射还是出射
    bool enter = wo.z > 0;

    // 如果光线为出射，交换介质两边的折射率与法线
    double   etai = enter ? etaA : etaB;
    double   etat = enter ? etaB : etaA;
    double   eta  = etai / etat;
    Normal3f n    = enter ? Normal3f(0, 0, 1) : Normal3f(0, 0, -1);

    pdf                                 = 1 - F;
    std::optional<Vector3f> wi_optional = Refract(wo, n, eta);
    if (!wi_optional)
      return Spectrum(0);

    wi = wi_optional.value();
    return ft / AbsCosTheta(wi);
  }
}

}  // namespace Raven