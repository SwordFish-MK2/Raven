#ifndef _RAVEN_SHADING_MODEL_OREN_NAYAR_H_
#define _RAVEN_SHADING_MODEL_OREN_NAYAR_H_

#include"../core/base.h"
#include"../core/bxdf.h"

namespace Raven {
	//class OrenNayar :public BxDF {
	//private:
	//	double sigma;
	//	Spectrum albedo;
	//	double A;
	//	double B;

	//public:
	//	OrenNayar(Spectrum albedo, double sigma) :BxDF(BxDFType(Reflection)), albedo(albedo), sigma(sigma) {
	//		double sigmaPow = pow(sigma, 2);
	//		A = (1 - sigmaPow) * 0.5f / (sigmaPow + 0.33f);
	//		B = 0.45f * sigmaPow / (sigmaPow + 0.09f);
	//	}

	//	virtual Spectrum f(const vectorf3& wo, const vectorf3& wi)const;

	//    virtual Spectrum sampled_f(const vectorf3& wo, vectorf3& wi, const pointf2& sample, double* pdf)const;

	//	double pdf(const vectorf3& wo, const vectorf3& wi)const;

	//};

	class OrenNayar :public BxDF {
	private:
		double sigma;
		Spectrum albedo;
		double A;
		double B;

	public:
		OrenNayar(Spectrum albedo, double sigma) :BxDF(BxDFType(Reflection)), albedo(albedo), sigma(sigma) {
			double sigmaPow = pow(sigma, 2);
			A = 1.0 - (sigmaPow * 0.5f / (sigmaPow + 0.33f));
			B = 0.45f * sigmaPow / (sigmaPow + 0.09f);
		}

		virtual Spectrum f(const Vector3f& wo, const Vector3f& wi)const;

		virtual Spectrum sampled_f(const Vector3f& wo, Vector3f& wi, const Point2f& sample, double& pdf)const;

		double pdf(const Vector3f& wo, const Vector3f& wi)const;

	};
}

#endif