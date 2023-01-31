#include<Raven/light/areaLight.h>

namespace Raven {
	Spectrum DiffuseAreaLight::Li(const SurfaceInteraction& p, const Vector3f& wi)const {
		return Dot(p.n, wi) > 0.0 ? emittedRadiance : Spectrum(0.0);
		//return emittedRadiance;
	}

	Spectrum DiffuseAreaLight::sampleLi(const SurfaceInteraction& inter, const Point2f& uv,
		LightSample* lightSample)const {
		//�ڹ�Դ�������һ���㣬����Ӹõ������p�ķ�������
		auto [lightInter, pdf] = shape_ptr->sample(inter, uv);
		if (pdf == 0 || DistanceSquared(lightInter.p, inter.p) == 0) {
			return Spectrum(0);
		}
		//����Shape��pdf������������õ��pdf
		lightSample->pdf = pdf;
		lightSample->wi = Normalize(lightInter.p - inter.p);//����Ϊ�ӵ�p�����Դ
		//lightSample->n = lightInter.n;
		lightSample->p = lightInter.p;
		return Li(lightInter, -(lightSample->wi));
	}


	//Vector3f DiffuseAreaLight::sample_Li(const SurfaceInteraction& inter, const Point2f& uv,
	//	Vector3f* wi, double* pdf, SurfaceInteraction* lightSample)const {
	//	//�ڹ�Դ�������һ���㣬����Ӹõ������p�ķ�������
	//	SurfaceInteraction lightInter = shape_ptr->sample(inter, uv);
	//	*wi = Normalize(lightInter.p - inter.p);//����Ϊ�ӵ�p�����Դ
	//	//����Shape��pdf������������õ��pdf
	//	*pdf = shape_ptr->pdf(lightInter);
	//	*lightSample = lightInter;
	//	return	Li(lightInter, -*wi);
	//}

	Spectrum DiffuseAreaLight::power()const {
		return emittedRadiance * area * M_PI;
	}

	double DiffuseAreaLight::pdf_Li(const SurfaceInteraction& inter, const Vector3f& wi)const {
		return shape_ptr->pdf(inter, wi);
	}

	DiffuseAreaLightReg DiffuseAreaLightReg::regHelper;
}