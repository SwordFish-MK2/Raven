#include"microfacet.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include<math.h>

namespace Raven {
	double MicrofacetDistribution::G1(const Vector3f& wo)const {
		return 1.0 / (1.0 + lambda(wo));
	}

	double MicrofacetDistribution::G2(const Vector3f& wo, const Vector3f& wi)const {
		return 1.0 / (1.0 + lambda(wo) + lambda(wi));
	}

	//given a pair of direction,compute half vector wh, return ndf value of wh
	double MicrofacetDistribution::pdf(const Vector3f& wo, const Vector3f& wi)const {
		Vector3f wh = Normalize(wo + wi);
		double cosTheta = abs(CosTheta(wh));
		return cosTheta * NDF(wh);
	}

	//normal distribution function of microfacets
	double BeckmannSpizzichino::NDF(const Vector3f& wh)const {
		//normal distribution function of Beckman-Spizzchino model
		double tan2Thetah = Tan2Theta(wh);

		if (std::isinf(tan2Thetah))
			return 1.0;

		double cos4Thetah = Cos2Theta(wh) * Cos2Theta(wh);
		double invScaler = 1 / (M_PI * alphaX * alphaY * cos4Thetah);

		if (alphaX == alphaY) {
			//isotropic case
			return exp(-tan2Thetah / alphaX * alphaY) * invScaler;

		}
		else {
			//anisotropic case
			double cos2Phih = Cos2Phi(wh);
			double sin2Phih = Sin2Phi(wh);

			double alphaX2 = alphaX * alphaX;
			double alphaY2 = alphaY * alphaY;

			double t = cos2Phih / alphaX2 + sin2Phih / alphaY2;
			return exp(-tan2Thetah * t) * invScaler;
		}

	}

	//auxiliary function to compute G1 and G2
	double BeckmannSpizzichino::lambda(const Vector3f& w)const {
		double absTantheta = AbsTanTheta(w);
		if (std::isinf(absTantheta))
			return 0;

		double alpha = sqrt(Cos2Phi(w) * (alphaX * alphaX) +
			Sin2Phi(w) * (alphaY * alphaY));

		double a = 1 / (alpha * absTantheta);

		if (a >= 1.6f)
			return 0;
		return (1 - 1.259f * a + 0.396f * a * a) /
			(3.535f * a + 2.181f * a * a);
	}

	//sample overall directions of upper hemisphere respect to ndf,return unit sampled normal vector
	Vector3f BeckmannSpizzichino::sample_wh(const Vector3f& wo, const Point2f& uv)const {
		if (alphaX == alphaY) {
			//isotropic case

			double logSample = log(1 - uv[0]);
			if (std::isinf(logSample))
				logSample = 0;

			double tanTheta2 = -alphaX * alphaX * logSample;
			double phi = 2 * M_PI * uv[1];

			double cosTheta = 1 / sqrt(1 + tanTheta2);
			double sinTheta = sqrt(1 - cosTheta * cosTheta);

			double x = sinTheta * cos(phi);
			double y = sinTheta * sin(phi);

			Vector3f wh = Vector3f(x, y, cosTheta);
			if (Dot(wo, wh) < 0)
				wh = -wh;
			return wh;
		}
		else {
			//antisotropic
			double alpha = alphaY / alphaX;
			double phi = atan(alpha * tan(2 * M_PI * uv[1] + 0.5f * M_PI));
			if (uv[1] > 0.5f)
				phi += M_PI;

			double cosPhi = cos(phi);
			double sinPhi = sin(phi);
			double alphaX2 = alphaX * alphaX;
			double alphaY2 = alphaY * alphaY;
			double t = cosPhi * cosPhi / alphaX2 + sinPhi * sinPhi / alphaY2;

			double logSample = log(1 - uv[0]);

			double theta = -logSample / t;

			double x = sin(theta) * cosPhi;
			double y = sin(theta) * sinPhi;
			double z = cos(theta);

			Vector3f wh = Vector3f(x, y, z);
			if (Dot(wh, wo) < 0)
				wh = -wh;
			return wh;
		}
	}

	double GGX::NDF(const Vector3f& wh)const {
		//normal distribution function of GGX model
		// 
		//if alphaX == alphaY, the distribution is isotropic
		double tan2 = Tan2Theta(wh);
		if (std::isinf(tan2))
			return 0;

		double cos2Thetah = Cos2Theta(wh);
		double cos4 = cos2Thetah * cos2Thetah;

		double cos2Phih = Cos2Phi(wh);
		double sin2Phih = Sin2Phi(wh);
		double alphaX2 = alphaX * alphaX;
		double alphaY2 = alphaY * alphaY;

		double e = tan2 * (cos2Phih / alphaX2 + sin2Phih / alphaY2);

		return 1.0 / (M_PI * alphaX * alphaY * cos4 * (1 + e) * (1 + e));

	}

	double GGX::lambda(const Vector3f& w)const {
		double tan = AbsTanTheta(w);

		if (std::isinf(tan))
			return 0.0;

		double tan2 = tan * tan;

		double cos2Phih = Cos2Phi(w);
		double sin2Phih = Sin2Phi(w);

		double alpha = sqrt(cos2Phih * alphaX * alphaX +
			sin2Phih * alphaY * alphaY);

		return 0.5 * (-1. + sqrt(1. + alpha * alpha * tan2));
	}

	Vector3f GGX::sample_wh(const Vector3f& wo, const Point2f& uv)const {
		if (alphaX == alphaY) {
			//isotropic
			double phi = 2 * M_PI * uv[0];

			double tanTheta2 = uv[1] * alphaX * alphaX / (1.0 - uv[1]);
			double cosTheta = 1.0 / sqrt(1 + tanTheta2);//tan2 + 1 = sec2
			double sinTheta = Max(0.0, sqrt(1 - cosTheta * cosTheta));

			Vector3f wh = Normalize(SphericalDirection(sinTheta, cosTheta, phi));
			if (!Dot(wh, wo) < 0)
				wh = -wh;
			return wh;
		}
		else {
			double alpha = alphaY / alphaX;
			double phi = atan(alpha * tan(2 * M_PI * uv[0] + 0.5 * M_PI));
			if (uv[0] > 0.5)
				phi += M_PI;

			double sinPhi = sin(phi), cosPhi = cos(phi);
			double alphaX2 = alphaX * alphaX;
			double alphaY2 = alphaY * alphaY;

			double t = cosPhi * cosPhi / alphaX2 + sinPhi * sinPhi / alphaY2;

			double tanTheta2 = uv[1] / (t * (1 - uv[1]));

			double cosTheta = 1 / sqrt(1 + tanTheta2);
			double sinTheta = Max(0.0, sqrt(1 - cosTheta * cosTheta));

			Vector3f wh = Vector3f(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
			if (Dot(wh, wo) < 0)
				wh = -wh;
			return wh;
		}
	}

	MicrofacetReflection::MicrofacetReflection(std::shared_ptr<Fresnel> fresnel,
		std::shared_ptr<MicrofacetDistribution> microfacet, Vector3f albedo) :
		BxDF(BxDFType(Reflection | Glossy)), fresnel(fresnel), microfacet(microfacet), albedo(albedo) {}

	//compute brdf value of given wo and wi directions
	Vector3f MicrofacetReflection::f(const Vector3f& wo, const Vector3f& wi)const {
		//compute brdf value
		Vector3f wh = wo + wi;

		double cosThetaO = abs(CosTheta(wo));
		double cosThetaI = abs(CosTheta(wi));
		if (cosThetaO == 0 || cosThetaI == 0)
			return Vector3f(0.0);

		if (wh == Vector3f(0.0))
			return Vector3f(0.0);

		wh = wh.normalized();

		if (Dot(wh, Vector3f(0.0, 0.0, 1.0)) < 0)
			wh = -wh;

		double D = microfacet->NDF(wh);
		double G = microfacet->G2(wo, wi);
		double F = fresnel->evaluate(Dot(wi, wh));

		return albedo * F * G * D / (4 * cosThetaO * cosThetaI);
	}

	//sample wi from microfacet distribution, compute corresponding pdf value, conpute and return brdf value 
	Vector3f MicrofacetReflection::sampled_f(const Vector3f& wo, Vector3f& wi, 
		const Point2f& uv, double* pdf)const {
		//generate wh sample with respect to ndf and given random numbers
		Vector3f wh = microfacet->sample_wh(wo, uv);
		wi = Reflect(wo, wh);
		//compute pdf

		if (!SameHemisphere(wi, wo))
			return Vector3f(0.0);

		*pdf = microfacet->pdf(wo, wi) / (4 * Dot(wo, wh));
		Vector3f v= f(wo, wi);
		return v;
	}

	double MicrofacetReflection::pdf(const Vector3f& wo, const Vector3f& wi)const {
		Vector3f wh = Normalize(wo + wi);
		return microfacet->pdf(wo, wi) / (4 * Dot(wo, wh));
	}

	//MicrofacetTransmission::MicrofacetTransmission(std::shared_ptr<Fresnel> fresnel,
	//	std::shared_ptr<MicrofacetDistribution> microfacet,
	//	double etaA, double etaB, Vector3f albedo) :
	//	BxDF(BxDFType(Transmission | Glossy)), fresnel(fresnel), microfacet(microfacet),
	//	etaA(etaA), etaB(etaB), albedo(albedo) {}
	//
	//Vector3f MicrofacetTransmission::f(const Vector3f& wo, const Vector3f& wi)const {
	//	if (wo == wi)
	//		return Vector3f(0.0);
	//	double eta = etaA / etaB;
	//	Vector3f wh = 0.5 * (wo + wi);
	//	double cosThetaO = abs(CosTheta(wo));
	//	double cosThetaI = abs(CosTheta(wi));
	//	if (cosThetaO == 0 || cosThetaI == 0)
	//		return Vector3f(0.0);
	//	double cosThetaHO = Dot(wh, wo);
	//	double cosThetaHI = Dot(wh, wi);
	//	if (cosThetaHO == 0. || cosThetaHI == 0.)
	//		return Vector3f(0.0);
	//	double D = microfacet->NDF(wh);
	//	double G = microfacet->G2(wo, wi);
	//	double F = (1. - fresnel->evaluate(cosThetaHO));
	//	double temp = pow(cosThetaHO + eta * cosThetaHI, 2);
	//	return albedo * D * G * F * abs(cosThetaO) * abs(cosThetaI) / (temp * cosThetaO * cosThetaI);
	//
	//}

}