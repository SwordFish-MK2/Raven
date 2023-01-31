#include<Raven/shading/microfacet.h>

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

	double BeckmannSpizzichino::pdf(const Vector3f& wo, const Vector3f& wi)const {
		Vector3f wh = Normalize(wi + wo);
		if (!sampleVNDF) {
			double cosThetah = CosTheta(wh);
			double D = NDF(wh);
			return D * cosThetah;
		}
		else {
			double G = G1(wo);
			double D = NDF(wh);
			return D * G * abs(Dot(wo, wh)) / AbsCosTheta(wo);
		}
	}

	//normal distribution function of microfacets
	double BeckmannSpizzichino::NDF(const Vector3f& wh)const {

		float cosTheta = CosTheta(wh);
		float sinTheta = std::max(0.f, sqrt(1 - cosTheta * cosTheta));
		float tanTheta = std::fabs(sinTheta / cosTheta);

		if (std::isinf(tanTheta))
			return 0.f;

		float tan2 = tanTheta * tanTheta;
		float cos4 = cosTheta * cosTheta * cosTheta * cosTheta;

		if (alphaX == alphaY) {
			float alpha2 = alphaX * alphaX;
			float D = exp(-tan2 / alpha2) / (M_PI * alpha2 * cos4);
			return D;
		}
		else {
			float cos2Phi = Cos2Phi(wh);
			float sin2Phi = Sin2Phi(wh);

			float alpha2 = cos2Phi / (alphaX * alphaX) +
				sin2Phi / (alphaY * alphaY);
			float D = exp(-tan2 / alpha2) / (M_PI * alphaX * alphaY * cos4);
			return D;
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
		if (!sampleVNDF) {
			if (alphaX == alphaY) {
				//isotropic case

				double logSample = log(1 - uv[0]);
				if (std::isinf(logSample))
					logSample = 0;

				double tanTheta2 = -alphaX * alphaX * logSample;
				double phi = 2 * M_PI * uv[1];

				double cosTheta = 1 / sqrt(1 + tanTheta2);
				double sinTheta = sqrt(Max(0.0, 1 - cosTheta * cosTheta));

				double x = sinTheta * cos(phi);
				double y = sinTheta * sin(phi);

				Vector3f wh = Vector3f(x, y, cosTheta);
				//确保采样的wh位于上半球
				if (!SameHemisphere(wo, wh))
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
		else {
			//拉伸入射光线
			Vector3f woStretched = Normalize(Vector3f(wo.x * alphaX, wo.y * alphaY, wo.z));

			auto [slopeX, slopeY] = BeckmannSample11(woStretched, uv);

			// 3. rotate
			double tmp = CosPhi(woStretched) * slopeX - SinPhi(woStretched) * slopeY;
			slopeY = SinPhi(woStretched) * slopeX + CosPhi(woStretched) * slopeY;
			slopeX = tmp;

			// 4. unstretch
			slopeX = alphaX * slopeX;
			slopeY = alphaY * slopeY;

			// 5. compute normal
			return Normalize(Vector3f(-slopeX, -slopeY, 1.f));

		}
	}

	double GGX::pdf(const Vector3f& wo, const Vector3f& wi)const {
		Vector3f wh = Normalize(wo + wi);
		if (!sampleVNDF) {
			double D = NDF(wh);
			double cosThetah = AbsCosTheta(wh);
			return D * cosThetah;
		}
		else {
			double D = NDF(wh);
			double G = G1(wo);
			return D * G * abs(Dot(wo, wh)) / AbsCosTheta(wo);
		}
	}

	double GGX::NDF(const Vector3f& wh)const {

		double tan2 = Tan2Theta(wh);
		if (std::isinf(tan2))
			return 0;

		double cos2Thetah = Cos2Theta(wh);
		double cos4 = cos2Thetah * cos2Thetah;

		double cos2Phih = Cos2Phi(wh);
		double sin2Phih = Sin2Phi(wh);
		double alphaX2 = alphaX * alphaX;
		double alphaY2 = alphaY * alphaY;

		double e = tan2 / alphaX2;

		return 1.0 / (M_PI * alphaX2 * cos4 * (1 + e) * (1 + e));
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

		return (-1.0 + sqrt(1.0 + alpha * alpha * tan2)) / 2;
	}

	Vector3f GGX::sample_wh(const Vector3f& wo, const Point2f& uv)const {
		if (!sampleVNDF) {

			//Sample whole hemisphere
			if (alphaX == alphaY) {
				//各项同性
				double phi = 2 * M_PI * uv[1];

				double tanTheta2 = uv[0] * alphaX * alphaX / (1.0 - uv[0]);
				double cosTheta = 1.0 / sqrt(1 + tanTheta2);//tan2 + 1 = sec2
				double sinTheta = sqrt(Max(0.0, 1 - cosTheta * cosTheta));

				Vector3f wh = Normalize(SphericalDirection(sinTheta, cosTheta, phi));
				if (!SameHemisphere(wo, wh))
					wh = -wh;
				return wh;
			}
			else {
				double alpha = alphaY / alphaX;
				double phi = atan(alpha * tan(2 * M_PI * uv[1] + 0.5 * M_PI));
				if (uv[1] > 0.5)
					phi += M_PI;

				double sinPhi = sin(phi), cosPhi = cos(phi);
				double alphaX2 = alphaX * alphaX;
				double alphaY2 = alphaY * alphaY;

				double t = cosPhi * cosPhi / alphaX2 + sinPhi * sinPhi / alphaY2;

				double tanTheta2 = uv[0] / (t * (1 - uv[0]));

				double cosTheta = 1 / sqrt(1 + tanTheta2);
				double sinTheta = sqrt(Max(0.0, 1 - cosTheta * cosTheta));

				Vector3f wh = Vector3f(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
				if (Dot(wh, wo) < 0)
					wh = -wh;
				return wh;
			}
		}
		else {
			//采样可见法线
			Vector3f woStreched = Normalize(Vector3f(wo.x * alphaX, wo.y * alphaY, wo.z));

			Vector3f t1 = (woStreched.z < 0.9999) ? Normalize(Cross(woStreched, Vector3f(0, 0, 1))) : Vector3f(1, 0, 0);
			Vector3f t2 = Normalize(Cross(t1, woStreched));
			//auto [t1, t2] = genTBN(woStreched);

			double a = 1.0 / (1.0 + woStreched.z);

			double r = sqrt(uv[1]);
			double phi = (uv[0] < a) ? uv[0] / a * M_PI :
				(uv[0] - a) / (1.0 - a) * M_PI + M_PI;

			double p1 = r * cos(phi);
			double p2 = r * sin(phi);
			if (uv[0] >= a)
				p2 *= woStreched.z;

			Vector3f n = p1 * t1 + p2 * t2 + sqrt(Max(0.0, 1.0 - p1 * p1 - p2 * p2)) * woStreched;

			Vector3f wh = Normalize(Vector3f(alphaX * n.x, alphaY * n.y, Max(0.0, n.z)));
			if (!SameHemisphere(wh, wo))
				wh = -wh;
			return wh;
		}
	}

	MicrofacetReflection::MicrofacetReflection(std::shared_ptr<Fresnel> fresnel,
		std::shared_ptr<MicrofacetDistribution> microfacet, Spectrum albedo) :
		BxDF(BxDFType(Reflection | Glossy)), fresnel(fresnel), microfacet(microfacet), albedo(albedo) {}

	//compute brdf value of given wo and wi directions
	Spectrum MicrofacetReflection::f(const Vector3f& wo, const Vector3f& wi)const {
		//compute brdf value
		Vector3f wh =Normalize(wo + wi);

		double cosThetaO = abs(CosTheta(wo));
		double cosThetaI = abs(CosTheta(wi));
		if (cosThetaO == 0 || cosThetaI == 0)
			return Spectrum(0.0);

		if (wh == Vector3f(0.0))
			return Spectrum(0.0);

		wh = wh.normalized();

		if (Dot(wh, Vector3f(0.0, 0.0, 1.0)) < 0)
			wh = -wh;

		double D = microfacet->NDF(wh);
		double G = microfacet->G2(wo, wi);
		double F = fresnel->evaluate(Dot(wi, wh));

		return albedo * F * G * D / (4 * cosThetaO * cosThetaI);
	}

	//采样wi，并计算brdf
	Spectrum MicrofacetReflection::sampled_f(
		const Vector3f& wo,
		Vector3f& wi,
		const Point2f& uv,
		double& pdf)const {
		//generate wh sample with respect to ndf and given random numbers
		Vector3f wh = microfacet->sample_wh(wo, uv);
		wi = Reflect(wo, wh);

		//compute pdf
		if (!SameHemisphere(wi, wo))
			return Spectrum(0.0);

		pdf = microfacet->pdf(wo, wi) / (4 * Dot(wo, wh));
		Spectrum v = f(wo, wi);
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

	std::tuple<double, double> BeckmannSample11(const Vector3f& wo, const Point2f& uv) {
		double invSqrtPI = 1.0 / sqrt(M_PI);

		double tanTheta = TanTheta(wo);
		double cotTheta = 1 / tanTheta;

		double a = -1, c = erf(cotTheta);

		double thetaO = atan(tanTheta);

		double fit = 1.0 + thetaO * (-0.876 + thetaO * (0.4265 - thetaO * 0.0594));
		double b = c - (1.0 + c) * std::pow(1.0 - uv[0], fit);

		float normalization = 1.0f / (1.0f + c +
			invSqrtPI * tanTheta * std::exp(-cotTheta * cotTheta));

		while (true) {
			if (!(b >= a || b <= c))
				b = 0.5f * (a + c);

			double invErf = ErfInv(b);
			double value = normalization * (1.0f + b +
				invSqrtPI * tanTheta * std::exp(-invErf * invErf)) - uv[0];

			if (abs(value) < 1e-5)
				break;

			if (value > 0.0)
				c = b;
			else
				a = b;

			double derivative = (1 - invErf * tanTheta) * normalization;
			b -= value / derivative;
		}

		double slopeX = ErfInv(b);
		double slopeY = ErfInv(2.0 * uv[1] - 1.0);

		return std::tuple<double, double>(slopeX, slopeY);
	}

}