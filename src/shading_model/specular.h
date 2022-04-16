#ifndef _RAVEN_SHADING_MODLE_SPECULAR_H_
#define _RAVEN_SHADING_MODEL_SPECULAR_H_

#include"../core/base.h"
#include"../core/bxdf.h"
#include"../core/spectrum.h"


namespace Raven {
	/// <summary>
	/// 镜面反射
	/// </summary>
	class SpecularReflection final :public BxDF {

	public:
		SpecularReflection(const Spectrum& r, const std::shared_ptr<Fresnel>& f) :
			BxDF(BxDFType(Reflection | Specular)), fresnel(f), r(r) {}
		virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const {
			return Spectrum(0.f);
		}
		Spectrum sampled_f(const Vector3f& wo, Vector3f& wi, const Point2f& sample, double& pdf)const;

		double pdf(const Vector3f& wo, const Vector3f& wi) const {
			return 1.0;
		}
	private:
		const Spectrum r;
		const std::shared_ptr<Fresnel> fresnel;
	};

	/// <summary>
	/// 高光透射
	/// </summary>
	class SpecularTransmission final :public BxDF {
	public:
		SpecularTransmission(const Spectrum& t, double etaI, double etaT) :
			BxDF(BxDFType(Specular | Transmission)), t(t), fresnel(etaI, etaT), etaA(etaI), etaB(etaT) {}

		Spectrum f(const Vector3f& wo, const Vector3f& wi)const {
			return Spectrum(0.0);
		}

		Spectrum sampled_f(const Vector3f& wo, Vector3f& wi, const Point2f& sample, double& pdf)const;

		double pdf(const Vector3f& wo, const Vector3f& wi)const {
			return 1.0;
		}

	private:
		double etaA, etaB;
		const Spectrum t;
		FresnelDielectric fresnel;
	};
}

#endif