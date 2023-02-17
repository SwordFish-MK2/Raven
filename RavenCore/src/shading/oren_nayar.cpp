#include<Raven/shading/oren_nayar.h>
#include<Raven/core/distribution.h>
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

	Spectrum OrenNayar::f(const Vector3f& wo, const  Vector3f& wi)const {
		double sinThetaI = SinTheta(wi);
		double sinThetaO = SinTheta(wo);
		//计算cos(phiI-phiO)
		double cosDelta = 0;
		if (sinThetaI > 1e-4 || sinThetaO > 1e-4) {
			double sinPhiI = SinPhi(wi);
			double sinPhiO = SinPhi(wo);
			double cosPhiI = CosPhi(wi);
			double cosPhiO = CosPhi(wo);
			cosDelta = Max(0.0, cosPhiI * cosPhiO + sinPhiI * sinPhiO);

		}
		//计算sinAlpha与tanBeta
		double cosThetaI = abs(CosTheta(wi));
		double cosThetaO = abs(CosTheta(wo));

		double sinAlpha = cosThetaI > cosThetaO ? sinThetaI : sinThetaO;
		double tanBeta = cosThetaI > cosThetaO ? sinThetaI / cosThetaI : sinThetaO / cosThetaO;
		Spectrum fr = albedo / M_PI * ((A + B * cosDelta) * sinAlpha * tanBeta);
		return fr;
	}

	Spectrum OrenNayar::sampled_f(const Vector3f& wo, Vector3f& wi, const Point2f& uv, double& pdf)const {
		if (wo.z < 0)return Spectrum(0.0);
		wi = CosWeightedSampleHemisphere(uv);
		double cosTheta = CosTheta(wi);
		pdf = CosWeightedHemispherePdf(cosTheta);
		return f(wo, wi);
	}

	double OrenNayar::pdf(const Vector3f& wo, const Vector3f& wi)const {
		double cosTheta = CosTheta(wi);
		return CosWeightedHemispherePdf(cosTheta);
	}
}