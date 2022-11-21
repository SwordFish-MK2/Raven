#ifndef _RAVEN_SHADING_MODEL_MICROFACET_H_
#define _RAVEN_SHADING_MODEL_MICROFACET_H_

#include<Raven/core/bxdf.h>
#include<Raven/core/base.h>
#include<tuple>
namespace Raven {

	class MicrofacetDistribution {
	public:
		virtual double NDF(const Vector3f& wh)const = 0;

		virtual Vector3f sample_wh(const Vector3f& wo, const Point2f& uv)const = 0;

		virtual double pdf(const Vector3f& wo, const Vector3f& wi)const;

		virtual double G2(const Vector3f& wo, const Vector3f& wi)const;

		virtual double G1(const Vector3f& w)const;
	protected:

		virtual double lambda(const Vector3f& wh)const = 0;
	};

	class BeckmannSpizzichino : public MicrofacetDistribution {
	private:
		double alphaX, alphaY;
		bool sampleVNDF;

		virtual double lambda(const Vector3f& wh)const;
	public:
		BeckmannSpizzichino(double alphaX, double alphaY,bool sampleVNDF=false) 
			:alphaX(Max(0.001, alphaX)), alphaY(Max(0.001, alphaY)),sampleVNDF(sampleVNDF) {}

		virtual double pdf(const Vector3f& wo, const Vector3f& wi)const;

		virtual double NDF(const Vector3f& wh)const;

		virtual Vector3f sample_wh(const Vector3f& wo, const Point2f& uv)const;

		static double RoughnessToAlpha(double roughness) {
			roughness = std::max(roughness, (double)1e-3);
			double x = std::log(roughness);
			return 1.62142f + 0.819955f * x + 0.1734f * x * x +
				0.0171201f * x * x * x + 0.000640711f * x * x * x * x;
		}
	};

	class GGX :public MicrofacetDistribution {
	private:
		double alphaX, alphaY;
		bool sampleVNDF;
		virtual double lambda(const Vector3f& wh)const;
	public:
		GGX(double alphaX, double alphaY, bool sampleVNDF = false)
			:alphaX(Max(0.001, alphaX)), alphaY(Max(0.001, alphaY)), sampleVNDF(sampleVNDF) {}

		virtual double NDF(const Vector3f& wh)const;

		virtual Vector3f sample_wh(const Vector3f& wo, const Point2f& uv)const;

		virtual double pdf(const Vector3f& wo, const Vector3f& wi)const;

		static double RoughnessToAlpha(double roughness);
	};

	class MicrofacetReflection :public BxDF {
	private:
		Spectrum albedo;
		std::shared_ptr<Fresnel> fresnel;
		std::shared_ptr<MicrofacetDistribution> microfacet;
	public:
		MicrofacetReflection(std::shared_ptr<Fresnel> fresnel,
			std::shared_ptr<MicrofacetDistribution> mircrofacet, Spectrum albedo);
		virtual Spectrum f(const Vector3f& wo, const Vector3f& wi)const;
		virtual Spectrum sampled_f(const Vector3f& wo, Vector3f& wi, const Point2f& uv, double& pdf)const;
		virtual double pdf(const Vector3f& wo, const Vector3f& wh)const;
	};

	//class MicrofacetTransmission :public BxDF {
	//private:
	//	Vector3f albedo;
	//	std::shared_ptr<Fresnel> fresnel;
	//	std::shared_ptr<MicrofacetDistribution> microfacet;
	//	double etaA, etaB;
	//public:
	//	MicrofacetTransmission(std::shared_ptr<Fresnel> fresnel, std::shared_ptr<MicrofacetDistribution> microfacet,
	//		double etaA, double etaB, Vector3f albedo);
	//	virtual Vector3f f(const Vector3f& wo, const Vector3f& wi)const;
	//	virtual Vector3f sampled_f(const Vector3f& wo, Vector3f& wi, const Point2f& uv, double* pdf)const {
	//		//TODO:FINISH Transmission
	//		return Vector3f(0.0);
	//	}
	//	double pdf(const Vector3f& wo, const Vector3f& wi)const {
	//		if (wo.z < 0)return 0.0;
	//		return microfacet->pdf(wo, wi);
	//	}
	//};

	inline double GGX::RoughnessToAlpha(double roughness) {
		roughness = Max(roughness, 1e-3);
		double x = std::log(roughness);
		return 1.62142f + 0.819955f * x + 0.1734f * x * x + 0.0171201f * x * x * x +
			0.000640711f * x * x * x * x;
	}

	std::tuple<double, double> BeckmannSample11(const Vector3f& wo, const Point2f& uv);
}

#endif