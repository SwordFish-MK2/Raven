#include"lambertain.h"

namespace Raven {

	//since lambertain scartter radiance uniformly info different directions,its brdf returns a constant value  
	Spectrum LambertainReflection::f(const Vector3f& wo, const Vector3f& wi)const {
		return albedo / M_PI;
	}

	//uniformly or cos weighted sample wi direction on the upper hemisphere, compute correspoding pdf value, compute and return brdf 
	Spectrum LambertainReflection::sampled_f(const Vector3f& wo, Vector3f& wi, const Point2f& sample, double* pdf)const {

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

	std::shared_ptr<LambertainReflection> LambertainReflection::build(const Spectrum& albedo) {
		return std::make_shared<LambertainReflection>(albedo);
	}

}