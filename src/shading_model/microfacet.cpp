#include"microfacet.h"
#define _USE_MATH_DEFINES
#include<math.h>

namespace Raven {
	//given a pair of direction,compute half vector wh, return ndf value of wh
	double MicrofacetDistribution::pdf(const Vector3f& wo, const Vector3f& wi)const {
		Vector3f wh = Normalize(wo + wi);
		double cosTheta = CosTheta(wh);
		return cosTheta * NDF(wh);
	}

	//normal distribution function of microfacets
	double BeckmannSpizzichino::NDF(const Vector3f& wh)const {
		//normal distribution function of Beckman-Spizzchino model
		double tanThetah = AbsTanTheta(wh);
		double cos4Thetah = pow(CosTheta(wh), 4);
		double tan2Thetah = tanThetah * tanThetah;
		double invScaler = M_PI * alphaX * alphaY * cos4Thetah;
		double scaler = 1 / invScaler;
		if (alphaX != alphaY) {
			//anisotropic case
			double cos2Phih = Cos2Phi(wh);
			double sin2Phih = Sin2Phi(wh);
			double power = -tan2Thetah * (cos2Phih / (alphaX * alphaX) + sin2Phih / (alphaY * alphaY));
			return scaler * exp(power);
		}
		else {
			//isotropic case
			double power = -tan2Thetah / (2 * alphaX * alphaX);
			return scaler * exp(power);
		}

	}

	//masking and shadowing term, ratio of microfacets that could be seen from both wo and wi direction
	double BeckmannSpizzichino::G2(const Vector3f& wo, const Vector3f& wi)const {
		return 1.f / (1.f + lambda(wo) + lambda(wi));
	}

	//masking and shadowding term, ratio of microfacets that could be seen from w direction
	double BeckmannSpizzichino::G1(const Vector3f& w)const {
		return 1.0 / (1.0 + lambda(w));
	}

	//auxiliary function to compute G1 and G2
	double BeckmannSpizzichino::lambda(const Vector3f& w)const {
		double cos2Phih = Cos2Phi(w);
		double sin2Phih = Sin2Phi(w);
		double alpha = sqrt(cos2Phih * alphaX * alphaX + sin2Phih * alphaY * alphaY);
		double tanTheta = AbsTanTheta(w);
		double a = 1 / (alpha * tanTheta);
		//using rational polynomial appximation to compute lamda funtion
		if (a >= 1.6f)
			return 0;
		return (1 - 1.259f * a + 0.396f * a * a) /
			(3.535f * a + 2.181f * a * a);
	}

	//sample overall directions of upper hemisphere respect to ndf,return unit sampled normal vector
	Vector3f BeckmannSpizzichino::sample_wh(const Point2f& uv)const {
		double logSample = log(1 - uv[0]);
		if (alphaX == alphaY) {//isotropic case
			double tanTheta2 = -(alphaX * alphaX) * logSample;
			double phi = 2 * M_PI * uv[1];
			double cosTheta2 = 1 / (1 - tanTheta2);
			double cosTheta = sqrt(cosTheta2);
			double sinTheta = sqrt(1 - cosTheta2);
			double x = sinTheta * cos(phi);
			double y = sinTheta * sin(phi);
			return Vector3f(x, y, cosTheta);
		}
		else {//antisotropic
			double phi = atan(alphaY / alphaX * tan(2 * M_PI * uv[1] + 0.5f * M_PI));
			double cosPhi = cos(phi);
			double sinPhi = sin(phi);
			double t = cosPhi * cosPhi / (alphaX * alphaX) + sinPhi * sinPhi / (alphaY * alphaY);
			double theta = -logSample / t;
			double x = sin(theta) * cosPhi;
			double y = sin(theta) * sinPhi;
			double z = cos(theta);
			return Vector3f(x, y, z);
		}
	}

	double GGX::NDF(const Vector3f& wh)const {
		//normal distribution function of GGX model
		//if alphaX == alphaY, the distribution is isotropic
		double cos2Thetah = Cos2Theta(wh);
		double cos4 = cos2Thetah * cos2Thetah;
		double tan = AbsTanTheta(wh);
		double tan2 = tan * tan;
		double cos2Phih = Cos2Phi(wh);
		double sin2Phih = Sin2Phi(wh);
		double alphaX2 = alphaX * alphaX;
		double alphaY2 = alphaY * alphaY;
		return 1.0 / (M_PI * alphaX * alphaY * cos4 * (1 + tan2 * (cos2Phih / alphaX2 + sin2Phih / alphaY2)));

	}

	double GGX::lambda(const Vector3f& w)const {
		double tan = AbsTanTheta(w);
		double tan2 = tan * tan;
		double cos2Phih = Cos2Phi(w);
		double sin2Phih = Sin2Phi(w);
		double alpha = sqrt(cos2Phih * alphaX * alphaX + sin2Phih * alphaY * alphaY);
		return 0.5 * (-1. + sqrt(1. + alpha * alpha * tan2));
	}

	double GGX::G1(const Vector3f& w)const {
		return 1.0 / (1.0 + lambda(w));
	}

	double GGX::G2(const Vector3f& wo, const Vector3f& wi)const {
		return 1.0 / (1.0 + lambda(wo) + lambda(wi));
	}

	Vector3f GGX::sample_wh(const Point2f& uv)const {
		if (alphaX == alphaY) {
			//isotropic
			double phi = 2 * M_PI * uv[0];
			double tanTheta2 = uv[1] * alphaX * alphaX / (1 - uv[1]);
			double cosTheta = 1 / sqrt(1 + tanTheta2);
			double sinTheta = sqrt(1 - cosTheta * cosTheta);//tan2 + 1 = sec2
			return Vector3f(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
		}
		else {
			double phi = atan(alphaY / alphaX * tan(2 * M_PI * uv[0] + 0.5 * M_PI));
			double tanTheta2 = uv[1] / (cos(phi) * cos(phi) / (alphaX * alphaX) + sin(phi) * sin(phi) / (alphaY * alphaY));
			double cosTheta = 1 / sqrt(1 + tanTheta2);
			double sinTheta = sqrt(1 - cosTheta * cosTheta);
			return Vector3f(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
		}
	}

	//MicrofacetReflection::MicrofacetReflection(std::shared_ptr<Fresnel> fresnel, 
	//	std::shared_ptr<MicrofacetDistribution> microfacet, Spectrum albedo) :
	//	BxDF(BxDFType(Reflection | Glossy)), fresnel(fresnel), microfacet(microfacet), albedo(albedo) {}

	////compute brdf value of given wo and wi directions
	//Spectrum MicrofacetReflection::f(const vectorf3& wo, const vectorf3& wi)const {
	//	//compute brdf value
	//	if (wo == wi)
	//		return Spectrum(0.0);
	//	vectorf3 wh = Normalize(wo + wi);
	//	double cosThetaO = abs(CosTheta(wo));
	//	double cosThetaI = abs(CosTheta(wi));
	//	if (cosThetaO == 0 || cosThetaI == 0)
	//		return Spectrum(0.0);
	//	cosThetaO = Clamp(cosThetaO, 1e-6, 0.0);
	//	cosThetaI = Clamp(cosThetaI, 1e-6, 0.0);
	//	double D = microfacet->NDF(wh);
	//	double G = microfacet->G2(wo, wi);
	//	double F = fresnel->evaluate(Dot(wo, wh));
	//	return albedo * F * G * D / Max(1e-6, (4 * cosThetaO * cosThetaI));
	//}

	////sample wi from microfacet distribution, compute corresponding pdf value, conpute and return brdf value 
	//Spectrum MicrofacetReflection::sampled_f(const vectorf3& wo, vectorf3& wi, const pointf2& uv, double* pdf) {
	//	//generate wh sample with respect to ndf and given random numbers
	//	vectorf3 wh = microfacet->sample_wh(uv);
	//	wi = Reflect(wo, normalf3(wh));
	//	//compute pdf
	//	//pdf = microfacet->pdf(wo, wi) / (4 * Dot(wo, wh));
	//	*pdf = microfacet->pdf(wo, wi);
	//	return f(wo, wi);
	//}

	//MicrofacetTransmission::MicrofacetTransmission(std::shared_ptr<Fresnel> fresnel,
	//	std::shared_ptr<MicrofacetDistribution> microfacet,
	//	double etaA, double etaB, Spectrum albedo) :
	//	BxDF(BxDFType(Transmission | Glossy)), fresnel(fresnel), microfacet(microfacet), etaA(etaA), etaB(etaB), albedo(albedo) {}

	//Spectrum MicrofacetTransmission::f(const vectorf3& wo, const vectorf3& wi)const {
	//	if (wo == wi)
	//		return Spectrum(0.0);
	//	double eta = etaA / etaB;
	//	vectorf3 wh = 0.5 * (wo + wi);
	//	double cosThetaO = abs(CosTheta(wo));
	//	double cosThetaI = abs(CosTheta(wi));
	//	if (cosThetaO == 0 || cosThetaI == 0)
	//		return Spectrum(0.0);
	//	double cosThetaHO = Dot(wh, wo);
	//	double cosThetaHI = Dot(wh, wi);
	//	if (cosThetaHO == 0. || cosThetaHI == 0.)
	//		return Spectrum(0.0);
	//	double D = microfacet->NDF(wh);
	//	double G = microfacet->G2(wo, wi);
	//	double F = (1. - fresnel->evaluate(cosThetaHO));
	//	double temp = pow(cosThetaHO + eta * cosThetaHI, 2);
	//	return albedo * D * G * F * abs(cosThetaO) * abs(cosThetaI) / (temp * cosThetaO * cosThetaI);

	//}

	MicrofacetReflection::MicrofacetReflection(std::shared_ptr<Fresnel> fresnel,
		std::shared_ptr<MicrofacetDistribution> microfacet, Vector3f albedo) :
		BxDF(BxDFType(Reflection | Glossy)), fresnel(fresnel), microfacet(microfacet), albedo(albedo) {}

	//compute brdf value of given wo and wi directions
	Vector3f MicrofacetReflection::f(const Vector3f& wo, const Vector3f& wi)const {
		//compute brdf value
		if (wo == wi)
			return Vector3f(0.0);
		Vector3f wh = Normalize(wo + wi);
		double cosThetaO = abs(CosTheta(wo));
		double cosThetaI = abs(CosTheta(wi));
		if (cosThetaO == 0 || cosThetaI == 0)
			return Vector3f(0.0);
		cosThetaO = Clamp(cosThetaO, 1e-6, 0.0);
		cosThetaI = Clamp(cosThetaI, 1e-6, 0.0);
		double D = microfacet->NDF(wh);
		double G = microfacet->G2(wo, wi);
		double F = fresnel->evaluate(Dot(wo, wh));
		return albedo * F * G * D / Max(1e-6, (4 * cosThetaO * cosThetaI));
	}

	//sample wi from microfacet distribution, compute corresponding pdf value, conpute and return brdf value 
	Vector3f MicrofacetReflection::sampled_f(const Vector3f& wo, Vector3f& wi, const Point2f& uv, double* pdf) {
		//generate wh sample with respect to ndf and given random numbers
		Vector3f wh = microfacet->sample_wh(uv);
		wi = Reflect(wo, Normal3f(wh));
		//compute pdf
		//pdf = microfacet->pdf(wo, wi) / (4 * Dot(wo, wh));
		*pdf = microfacet->pdf(wo, wi);
		return f(wo, wi);
	}

	MicrofacetTransmission::MicrofacetTransmission(std::shared_ptr<Fresnel> fresnel,
		std::shared_ptr<MicrofacetDistribution> microfacet,
		double etaA, double etaB, Vector3f albedo) :
		BxDF(BxDFType(Transmission | Glossy)), fresnel(fresnel), microfacet(microfacet), 
		etaA(etaA), etaB(etaB), albedo(albedo) {}

	Vector3f MicrofacetTransmission::f(const Vector3f& wo, const Vector3f& wi)const {
		if (wo == wi)
			return Vector3f(0.0);
		double eta = etaA / etaB;
		Vector3f wh = 0.5 * (wo + wi);
		double cosThetaO = abs(CosTheta(wo));
		double cosThetaI = abs(CosTheta(wi));
		if (cosThetaO == 0 || cosThetaI == 0)
			return Vector3f(0.0);
		double cosThetaHO = Dot(wh, wo);
		double cosThetaHI = Dot(wh, wi);
		if (cosThetaHO == 0. || cosThetaHI == 0.)
			return Vector3f(0.0);
		double D = microfacet->NDF(wh);
		double G = microfacet->G2(wo, wi);
		double F = (1. - fresnel->evaluate(cosThetaHO));
		double temp = pow(cosThetaHO + eta * cosThetaHI, 2);
		return albedo * D * G * F * abs(cosThetaO) * abs(cosThetaI) / (temp * cosThetaO * cosThetaI);

	}
}