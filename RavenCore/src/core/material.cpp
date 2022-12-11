#include<Raven/core/material.h>

namespace Raven {

	void Material::Bump(
		const Ref<Texture<double>>& bump,
		SurfaceInteraction& si)
	{
		//ͨ��bump map��ȡpPrime������dpPrime�������dpdu��dpdvȷ���µķ��߷���

		if (bump == nullptr)return;
		double dis = bump->evaluate(si);
		SurfaceInteraction d_si = si;

		//����ǰ���ּ���dBump_du

		//����deltaU�ĳ���
		double deltaU = .5 * (std::abs(si.dudx) + std::abs(si.dudy));
		if (deltaU == 0)deltaU = 0.01;

		//����bump(U+deltaU,V)
		d_si.p = si.p + si.dpdu * deltaU;
		d_si.uv = si.uv + Vector2f(du, 0);
		d_si.n = Normalize(Cross(si.shading.dpdu, si.shading.dpdv) + deltaU * si.dndu);
		double dis_u = bump->evaluate(d_si);

		//����dBump_du
		Vector3f dpdu = si.shading.dpdu +
			(dis_u - dis) / deltaU * Vector3f(si.shading.n) +
			dis * si.shading.dndu;

		//����ǰ���ּ���dBump_dv

		//����deltaV�ĳ���
		double ddeltaV = .5 * (std::abs(si.dvdx) + std::abs(si.dvdy));
		if (deltaV == 0)deltaV = 0.01;

		//����bump(U,V+deltaV)
		d_si.p = si.p + si.dpdv + deltaV;
		d_si.uv = si.uv + Vector2f(0.0, dv);
		d_si.n = Normalize(Cross(si.shading.dpdu, si.shading.dpdv) + deltaV * si.dndv);
		double dis_v = bump->evaluate(d_si);

		//����dBump_dv
		Vector3f dpdv = si.shading.dpdv +
			(dis_v - dis) / deltaV * Vector3f(si.shading.n) +
			dis * si.shading.dndv;

		//set shading property 
		si.setShadingGeometry(dpdu, dpdv, si.shading.dndu, si.shading.dndv);
	}

}