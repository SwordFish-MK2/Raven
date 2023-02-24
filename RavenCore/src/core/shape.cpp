#include <Raven/core/interaction.h>
#include <Raven/core/shape.h>

namespace Raven {

// default实现为在几何体上均匀的采样一个点，再将pdf转化为对面积的积分
std::tuple<SurfaceInteraction, double> Shape::sample(
    const Interaction& ref,
    const Point2f&     rand) const {
  // 在表面均匀的采样一个点并计算pdf
  auto [inter, pdf] = sample(rand);

  // 将pdf从对面积的积分转为对立体角的积分
  Vector3f wi              = inter.p - ref.p;
  double   distanceSquared = wi.lengthSquared();
  if (distanceSquared == 0)
    pdf = 0.0;
  else {
    wi  = Normalize(wi);
    pdf *= distanceSquared / abs(Dot(inter.n, -wi));
    if (std::isinf(pdf))
      pdf = 0.0;
  }
  return std::tuple<SurfaceInteraction, double>(inter, pdf);
}

// default实现为计算几何体上均匀采样的pdf，再乘以雅可比行列式
double Shape::pdf(const Interaction& inter, const Vector3f& wi) const {
  // intersect sample ray to light geometry
  Point3f origin = inter.p;
  Ray     r      = inter.scatterRay(wi);
  HitInfo hitInfo;
  if (!intersect(r, hitInfo))
    return 0;
  SurfaceInteraction lightInter = getGeoInfo(hitInfo.pHit);
  // convert the pdf from integral of light surface to integral of the solid
  // angle of sample point
  double pdf = DistanceSquared(lightInter.p, inter.p) /
               (std::abs(Dot(lightInter.n, -wi)) * area());
  return pdf;
}
}  // namespace Raven