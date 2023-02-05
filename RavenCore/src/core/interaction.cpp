#include <Raven/core/interaction.h>
#include <Raven/core/light.h>
namespace Raven {
Interaction::Interaction(const Point3f&         p,
                         const Normal3f&        n,
                         const Vector3f&        wo,
                         double                 time,
                         const MediumInterface& mediumInterface)
    : p(p),
      n(n),
      wo(Normalize(wo)),
      time(time),
      mediumInterface(mediumInterface),
      t(0) {}

Interaction::Interaction(const Point3f&  p,
                         const Normal3f& n,
                         const Vector3f& wo,
                         double          time,
                         double          t)
    : p(p), n(n), wo(Normalize(wo)), time(time), t(t) {}

Interaction::Interaction(const Point3f&         p,
                         const Vector3f&        wo,
                         double                 time,
                         double                 t,
                         const MediumInterface& mediumInterface)
    : p(p), wo(wo), time(time), t(t) {}

Interaction::Interaction(const Point3f&         p,
                         double                 time,
                         double                 t,
                         const MediumInterface& mediumInterface)
    : p(p), time(time), t(t), mediumInterface(mediumInterface) {}

const Ref<Medium> Interaction::getMedium(const Vector3f& w) const {
  return Dot(w, n) > 0 ? mediumInterface.outside : mediumInterface.inside;
}

const Ref<Medium> Interaction::getMedium() const {
  assert(mediumInterface.inside == mediumInterface.outside);
  return mediumInterface.inside;
}
SurfaceInteraction::SurfaceInteraction(const Point3f&  p,
                                       const Normal3f& n,
                                       const Vector3f& wo,
                                       const Point2f&  uv,
                                       const Vector3f& dpdu,
                                       const Vector3f& dpdv,
                                       const Vector3f& dndu,
                                       const Vector3f& dndv,
                                       double          time,
                                       double          t)
    : Interaction(p, Normal3f(Normalize(Cross(dpdu, dpdv))), wo, time, t),
      dpdu(dpdu),
      dpdv(dpdv),
      dndu(dndu),
      dndv(dndv) {
  // 生成shading geometry
  shading.n    = n;
  shading.dndu = dndu;
  shading.dndv = dndv;
  shading.dpdu = dpdu;
  shading.dpdv = dpdv;
}

MediumInteraction::MediumInteraction(const Point3f&            p,
                                     const Vector3f&           wo,
                                     double                    time,
                                     double                    t,
                                     const Ref<Medium>         medium,
                                     const Ref<PhaseFunction>& phase)
    : Interaction(p, wo, time, t, MediumInterface(medium)), phase(phase) {}

// 根据微分光线计算交点的参数坐标(u,v)分别关于屏幕空间坐标(x,y)的偏导数
void SurfaceInteraction::computeDifferential(const RayDifferential& rd) {
  // 光线不含有微分光线，则纹理与法线的偏导数全部置零
  if (!rd.hasDifferential) {
    dudx = 0;
    dudy = 0;
    dvdx = 0;
    dvdy = 0;
    dpdx = Vector3f(0.0);
    dpdy = Vector3f(0.0);
  } else {
    // 计算微分光线与场景的交点 px, py
    double  d  = -Dot(Vector3f(n), Vector3f(p));
    double  tx = (-Dot(n, Vector3f(rd.originX)) - d) / Dot(n, rd.directionX);
    Point3f px = rd.originX + rd.directionX * tx;
    double  ty = (-Dot(n, Vector3f(rd.originY)) - d) / Dot(n, rd.directionY);
    Point3f py = rd.originY + rd.directionY * ty;

    // 计算dpdx,dpdy,由于dx、dy为1,dpdx = dp,dpdy = dy
    dpdx = px - p;
    dpdy = py - p;

    // 选择相关性较强的两个维度求解线性方程组
    int dim[2];
    if (std::abs(n.x) > std::abs(n.y) && std::abs(n.x) > std::abs(n.z)) {
      dim[0] = 1;
      dim[1] = 2;
    } else if (std::abs(n.y) > std::abs(n.z)) {
      dim[0] = 0;
      dim[1] = 2;
    } else {
      dim[0] = 0;
      dim[1] = 1;
    }
    double m[]  = {dpdu[dim[0]], dpdv[dim[0]], dpdu[dim[1]], dpdv[dim[1]]};
    double bx[] = {dpdx[dim[0]], dpdx[dim[1]]};
    double by[] = {dpdy[dim[0]], dpdy[dim[1]]};

    // 求解线性方程组解得dudx、dvdx、dudy、dvdy
    if (!solve2x2LinearSystem(m, bx, &dudx, &dvdx)) {
      dudx = 0;
      dvdx = 0;
    }
    if (!solve2x2LinearSystem(m, by, &dudy, &dvdy)) {
      dvdx = 0;
      dvdy = 0;
    }
  }
}

void SurfaceInteraction::setShadingGeometry(const Vector3f& dpdu,
                                            const Vector3f& dpdv,
                                            const Vector3f& dndu,
                                            const Vector3f& dndv) {
  // compute shading.n
  shading.n = Normalize(Cross(dpdu, dpdv));
  shading.n = FaceForward(n, shading.n);

  shading.dpdu = dpdu;
  shading.dpdv = dpdv;
  shading.dndu = dndu;
  shading.dndv = dndv;
}

Ray SurfaceInteraction::scartterRay(const Vector3f& dir,bool reflection) const {
  Point3f ori = p + dir * 0.001;
  if(reflection)
    return Ray(ori,dir,0,mediumInterface.outside);
  else
    return Ray(ori,dir,0,mediumInterface.inside);
}

Spectrum SurfaceInteraction::Le(const Vector3f& w) const {
  if (hitLight) {
    return light->Li(*this, w);
  } else
    return Spectrum(0.0);
}
}  // namespace Raven
