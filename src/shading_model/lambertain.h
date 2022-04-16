#ifndef _RAVEN_SHADING_MODEL_LAMBERTAIN_H_
#define _RAVEN_SHADING_MODEL_LAMBERTAIN_H_

#include"../core/base.h"
#include"../core/math.h"
#include"../core/distribution.h"
#include"../core/bxdf.h"

namespace Raven {
	/// <summary>
	/// 理想漫反射模型
	/// </summary>
	class LambertainReflection :public BxDF {
	public:
		LambertainReflection(Spectrum albedo) :BxDF(BxDFType(Reflection | Diffuse)), albedo(albedo) {}

		virtual Spectrum f(const Vector3f& wo, const Vector3f& wi)const;

		virtual Spectrum sampled_f(const Vector3f& wo, Vector3f& wi, const Point2f& sample, double& pdf)const;

		double pdf(const Vector3f& wo, const Vector3f& wi)const;

		Spectrum hdf(const Vector3f& wo, int nSamples, Point2f* samples) { return  albedo; }

		Spectrum hhf(int nSamples, Point2f* outSamples, Point2f* inSamples) { return albedo; }

		static std::shared_ptr<LambertainReflection> build(const Spectrum& albedo);
	private:
		Spectrum albedo;
	};
}

#endif