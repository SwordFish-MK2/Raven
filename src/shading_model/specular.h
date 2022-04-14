#ifndef _RAVEN_SHADING_MODLE_SPECULAR_H_
#define _RAVEN_SHADING_MODEL_SPECULAR_H_

#include"../core/base.h"
#include"../core/bxdf.h"
#include"../core/spectrum.h"


namespace Raven {
	class SpecularReflection final :public BxDF {

	public:
		SpecularReflection(const Spectrum& r, const std::shared_ptr<Fresnel>& f) :
			BxDF(BxDFType(Reflection | Specular)), fresnel(f), r(r) {}
		virtual Spectrum f(const Vector3f& wo, const Vector3f& wi) const {
			return Spectrum(0.f);
		}
		virtual Spectrum sampled_f(const Vector3f& wo, Vector3f& wi, const Point2f& sample, double* pdf)const;

		double pdf(const Vector3f& wo, const Vector3f& wi) const {
			return 0;
		}
	private:
		const Spectrum r;
		const std::shared_ptr<Fresnel> fresnel;
	};

	class SpecularTransmission final :public BxDF {
	public:
		SpecularTransmission(const Spectrum& t, double etaI, double etaT);
	private:
		const Spectrum t;
		FresnelDielectric fresnel;
	};
}

#endif