#include"areaLight.h"

namespace Raven {
	Vector3f DiffuseAreaLight::Li(const SurfaceInteraction& p, const Vector3f& wi)const {
		return Dot(p.n, wi) > 0.0 ? emittedRadiance : Vector3f(0.0);
	}

	Vector3f DiffuseAreaLight::sampleLi(const SurfaceInteraction& inter, const Point2f& uv,
		LightSample* lightSample)const {
		//在光源表面采样一个点，算出从该点射向点p的方向向量
		SurfaceInteraction lightInter = shape_ptr->sample(inter, uv);
		//调用Shape的pdf函数求出采样该点的pdf
		lightSample->pdf = shape_ptr->pdf(lightInter);
		lightSample->wi = Normalize(lightInter.p - inter.p);//方向为从点p入射光源
		lightSample->n = lightInter.n;
		lightSample->p = lightInter.p;
		return	Li(lightInter, -(lightSample->wi));
	}

	Vector3f DiffuseAreaLight::power()const {
		return emittedRadiance * area * M_PI;
	}

	double DiffuseAreaLight::pdf_Li(const SurfaceInteraction& inter, const Vector3f& wi)const {
		return shape_ptr->pdf(inter, wi);
	}

}