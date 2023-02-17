#include <Raven/core/interaction.h>
#include <Raven/core/material.h>

namespace Raven {

void Material::Bump(const Ref<Texture<double>>& bump, SurfaceInteraction& si) {
  // 通过bump map获取pPrime，计算dpPrime点的切线dpdu，dpdv确定新的法线方向

  double             dis  = bump->evaluate(si);
  SurfaceInteraction d_si = si;

  // 利用前向差分计算dBump_du

  // 计算deltaU的长度
  double deltaU = .5 * (std::abs(si.dudx) + std::abs(si.dudy));
  if (deltaU == 0)
    deltaU = 0.01;

  // 计算bump(U+deltaU,V)
  d_si.p  = si.p + si.dpdu * deltaU;
  d_si.uv = si.uv + Vector2f(deltaU, 0);
  d_si.n =
      Normalize(Cross(si.shading.dpdu, si.shading.dpdv) + deltaU * si.dndu);
  double dis_u = bump->evaluate(d_si);

  // 计算dBump_du
  Vector3f dpdu = si.shading.dpdu +
                  (dis_u - dis) / deltaU * Vector3f(si.shading.n) +
                  dis * si.shading.dndu;

  // 利用前向差分计算dBump_dv

  // 计算deltaV的长度
  double deltaV = .5 * (std::abs(si.dvdx) + std::abs(si.dvdy));
  if (deltaV == 0)
    deltaV = 0.01;

  // 计算bump(U,V+deltaV)
  d_si.p  = si.p + si.dpdv * deltaV;
  d_si.uv = si.uv + Vector2f(0.0, deltaV);
  d_si.n =
      Normalize(Cross(si.shading.dpdu, si.shading.dpdv) + deltaV * si.dndv);
  double dis_v = bump->evaluate(d_si);

  // 计算dBump_dv
  Vector3f dpdv = si.shading.dpdv +
                  (dis_v - dis) / deltaV * Vector3f(si.shading.n) +
                  dis * si.shading.dndv;

  // set shading property
  si.setShadingGeometry(dpdu, dpdv, si.shading.dndu, si.shading.dndv);
}

}  // namespace Raven