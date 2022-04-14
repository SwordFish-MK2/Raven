#include"specular.h"

namespace Raven {

	Spectrum SpecularReflection::sampled_f(const Vector3f& wo, Vector3f& wi, const Point2f& sample, double* pdf)const {
		wi = Vector3f(-wo.x, -wo.y, wo.z);
		*pdf = 1.0;
		return fresnel->evaluate(CosTheta(wi)) * r / Max(0.0, CosTheta(wi));
	}

}