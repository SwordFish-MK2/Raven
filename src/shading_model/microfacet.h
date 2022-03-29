#ifndef _RAVEN_SHADING_MODEL_MICROFACET_H_
#define _RAVEN_SHADING_MODEL_MICROFACET_H_

#include"../core/bxdf.h"
#include"../core/base.h"

namespace Raven {

	class MicrofacetDistribution {
	public:
		virtual double NDF(const Vector3f& wh)const = 0;
		virtual double G2(const Vector3f& wo, const Vector3f& wi)const = 0;
		virtual double G1(const Vector3f& w)const = 0;
		virtual Vector3f sample_wh(const Point2f& uv)const = 0;
		virtual double lambda(const Vector3f& wh)const = 0;
		virtual double pdf(const Vector3f& wo, const Vector3f& wi)const;
	private:
		virtual double lamda(const Vector3f& wh)const = 0;
	};

	class BeckmannSpizzichino : public MicrofacetDistribution {
	private:
		double alphaX, alphaY;

		virtual double lambda(const Vector3f& wh)const;
	public:
		virtual double NDF(const Vector3f& wh)const;
		virtual double G2(const Vector3f& wo, const Vector3f& wi)const;
		virtual double G1(const Vector3f& w)const;
		virtual Vector3f sample_wh(const Point2f& uv)const;
	};

	class GGX :public MicrofacetDistribution {
	public:
		double alphaX, alphaY;

		virtual double lambda(const Vector3f& wh)const;
	private:
		virtual double NDF(const Vector3f& wh)const;
		virtual double G2(const  Vector3f& wo, const Vector3f& wi)const;
		virtual double G1(const Vector3f& w)const;
		virtual Vector3f sample_wh(const Point2f& uv)const;
	};

	class MicrofacetReflection :public BxDF {
	private:
		Vector3f albedo;
		std::shared_ptr<Fresnel> fresnel;
		std::shared_ptr<MicrofacetDistribution> microfacet;
	public:
		MicrofacetReflection(std::shared_ptr<Fresnel> fresnel, 
			std::shared_ptr<MicrofacetDistribution> mircrofacet, Vector3f albedo);
		virtual Vector3f f(const Vector3f& wo, const Vector3f& wi)const;
		virtual Vector3f sampled_f(const Vector3f& wo, Vector3f& wi, const Point2f& uv, double* pdf);
		virtual double pdf(const Vector3f& wo, const Vector3f& wi)const {
			Vector3f wh = Normalize(wo + wi);
			return microfacet->NDF(wh) * CosTheta(wh);
		}
	};

	class MicrofacetTransmission :public BxDF {
	private:
		Vector3f albedo;
		std::shared_ptr<Fresnel> fresnel;
		std::shared_ptr<MicrofacetDistribution> microfacet;
		double etaA, etaB;
	public:
		MicrofacetTransmission(std::shared_ptr<Fresnel> fresnel, std::shared_ptr<MicrofacetDistribution> microfacet,
			double etaA, double etaB, Vector3f albedo);
		virtual Vector3f f(const Vector3f& wo, const Vector3f& wi)const;
		virtual Vector3f sampled_f(const Vector3f& wo, Vector3f& wi, const Point2f& uv, double* pdf)const {
			//TODO:FINISH Transmission
			return Vector3f(0.0);
		}
		double pdf(const Vector3f& wo, const Vector3f& wi)const {
			if (wo.z < 0)return 0.0;
			return microfacet->pdf(wo, wi);
		}
	};

}

#endif