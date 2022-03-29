#include"lambertain.h"

namespace Raven {
	////since lambertain scartter radiance uniformly info different directions,its brdf returns a constant value  
	//Spectrum LambertainReflection::f(const vectorf3& wo, const vectorf3& wi)const {
	//	return albedo / M_PI;
	//}

	////uniformly or cos weighted sample wi direction on the upper hemisphere, compute correspoding pdf value, compute and return brdf 
	//Spectrum LambertainReflection::sampled_f(const vectorf3& wo, vectorf3& wi, const pointf2& sample, double* pdf)const {
	//	if (wo.z <= 0)
	//		return Spectrum(0.0);
	//	wi = Normalize(CosWeightedSampleHemisphere(sample));//cos weighted sample wi
	//	double cosTheta = CosTheta(wi);
	//	*pdf = CosWeightedHemispherePdf(cosTheta);//compute pdf value related to wi
	//	return f(wo, wi);
	//}

	////compute pdf value of given pair of directions
	//double LambertainReflection::pdf(const vectorf3& wo, const vectorf3& wi)const {
	//	//double cosTheta = CosTheta(wi);
	//	return UniformHemispherePdf();
	//}

		//since lambertain scartter radiance uniformly info different directions,its brdf returns a constant value  
	Vector3f LambertainReflection::f(const Vector3f& wo, const Vector3f& wi)const {
		return albedo / M_PI;
	}

	//uniformly or cos weighted sample wi direction on the upper hemisphere, compute correspoding pdf value, compute and return brdf 
	Vector3f LambertainReflection::sampled_f(const Vector3f& wo, Vector3f& wi, const Point2f& sample, double* pdf)const {

		wi = Normalize(CosWeightedSampleHemisphere(sample));//cos weighted sample wi
		double cosTheta = CosTheta(wi);
		*pdf = CosWeightedHemispherePdf(cosTheta);//compute pdf value related to wi

		if (wo.z <= 0)
			wi *= -1;
		return f(wo, wi);
	}

	//compute pdf value of given pair of directions
	double LambertainReflection::pdf(const Vector3f& wo, const Vector3f& wi)const {
		double cosTheta = CosTheta(wi);
		return CosWeightedHemispherePdf(cosTheta);
	}

	std::shared_ptr<LambertainReflection> LambertainReflection::build(const Vector3f& albedo) {
		return std::make_shared<LambertainReflection>(albedo);
	}
}