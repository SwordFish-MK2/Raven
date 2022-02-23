#include"oren_nayar.h"
#include"../core/distribution.h"
namespace Raven {
	//Spectrum OrenNayar::f(const vectorf3& wo,const  vectorf3& wi)const {
	//	//double thetaO = Clamp(acos(wo[2]), 0.f, M_PI);
	//	//double thetaI = Clamp(acos(wi[2]), 0.f, M_PI);
	//	double thetaO = Dot(wo, BRDFCoorNormal);
	//	double thetaI = Dot(wi, BRDFCoorNormal);
	//	double phiO = atan(wo[1] / wo[0]);
	//	if (phiO < 0.f)
	//		phiO += M_PI;
	//	double phiI = atan(wi[1] / wi[0]);
	//	if (phiI < 0.f)
	//		phiI += M_PI;
	//	double deltaPhi = phiI - phiO;
	//	double alpha = Max(thetaI, thetaO);
	//	double beta = Min(thetaI, thetaO);
	//	Spectrum fr = albedo / M_PI * (A + B * Max(0., cos(deltaPhi)) * sin(alpha) * tan(beta));
	//	return fr;
	//}

	//Spectrum OrenNayar::sampled_f(const vectorf3& wo, vectorf3& wi, const pointf2& uv, double* pdf)const {
	//	if (wo.z < 0)return Spectrum(0.0);
	//	wi = CosWeightedSampleHemisphere(uv);
	//	double cosTheta = CosTheta(wi);
	//	*pdf = CosWeightedHemispherePdf(cosTheta);
	//	return f(wo, wi);
	//}

	//double OrenNayar::pdf(const vectorf3& wo, const vectorf3& wi)const {
	//	double cosTheta = CosTheta(wi);
	//	return CosWeightedHemispherePdf(cosTheta);
	//}

	Vector3f OrenNayar::f(const Vector3f& wo, const  Vector3f& wi)const {
		//double thetaO = Clamp(acos(wo[2]), 0.f, M_PI);
		//double thetaI = Clamp(acos(wi[2]), 0.f, M_PI);
		double thetaO = Dot(wo, Vector3f(0.f, 0.f, 1.f));
		double thetaI = Dot(wi, Vector3f(0.f, 0.f, 1.f));
		double phiO = atan(wo[1] / wo[0]);
		if (phiO < 0.f)
			phiO += M_PI;
		double phiI = atan(wi[1] / wi[0]);
		if (phiI < 0.f)
			phiI += M_PI;
		double deltaPhi = phiI - phiO;
		double alpha = Max(thetaI, thetaO);
		double beta = Min(thetaI, thetaO);
		Vector3f fr = albedo / M_PI * (A + B * Max(0., cos(deltaPhi)) * sin(alpha) * tan(beta));
		return fr;
	}

	Vector3f OrenNayar::sampled_f(const Vector3f& wo, Vector3f& wi, const Point2f& uv, double* pdf)const {
		if (wo.z < 0)return Vector3f(0.0);
		wi = CosWeightedSampleHemisphere(uv);
		double cosTheta = CosTheta(wi);
		*pdf = CosWeightedHemispherePdf(cosTheta);
		return f(wo, wi);
	}

	double OrenNayar::pdf(const Vector3f& wo, const Vector3f& wi)const {
		double cosTheta = CosTheta(wi);
		return CosWeightedHemispherePdf(cosTheta);
	}
}