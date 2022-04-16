#ifndef _RAVEN_CORE_BXDF_H_
#define _RAVEN_CORE_BXDF_H_


#include"base.h"
#include"math.h"
#include"spectrum.h"
#include<optional>
namespace Raven {

	double FDielectric(double cosThetaI, double etaI, double etaT);

	double FConductor(double cosThetaI, double etaI, double etaT, double k);

	inline Vector3f reflect(const Vector3f& wo, const Normal3f& n = Normal3f(0, 0, 1)) {
		return -wo + n * Dot(wo, n) * 2.0;
	}

	inline Vector3f Reflect(const Vector3f& wo, const Vector3f& n = Vector3f(0, 0, 1)) {
		return -wo + n * Dot(wo, n) * 2.0;
	}

	inline Vector3f Reflect(const Vector3f& wo) {
			return Vector3f(-wo.x, -wo.y, wo.z);
	}

	//计算折射光方向，eta = etaI/etaT
	inline std::optional<Vector3f> Refract(const Vector3f& wo, const Vector3f& n, double eta) {

		double cosThetaI = Dot(wo, n);
		double sinThetaI2 = Max(0.0, 1 - cosThetaI * cosThetaI);
		double sinThetaT2 = eta * eta * sinThetaI2;

		if (sinThetaT2 >= 1)//全反射
			return std::nullopt;

		double cosThetaT = sqrt(1 - sinThetaT2);

		Vector3f wt = -eta * wo + (eta * cosThetaI - cosThetaT) * n;
		return wt;
	}

	inline std::optional<Vector3f> Refract(const Vector3f& wo, const Normal3f& n, double eta) {
		double cosThetaI = Dot(wo, n);
		double sinThetaI2 = Max(0.0, 1 - cosThetaI * cosThetaI);
		double sinThetaT2 = eta * eta * sinThetaI2;

		if (sinThetaT2 >= 1)//全反射
			return std::nullopt;

		double cosThetaT = sqrt(1 - sinThetaT2);

		Vector3f wt = -eta * wo + (eta * cosThetaI - cosThetaT) * n;
		return wt;
	}

	//fresnel class只用于计算反射的辐射度
	class Fresnel {
	public:
		virtual double evaluate(double cosTheta)const = 0;
	};

	class FresnelConductor :public Fresnel {
	private:
		//index of refraction of incident and emergent light etaI and etaT
		//complex-valued index of refraction k
		double etaI, etaT, k;
	public:
		FresnelConductor(double etai, double etat, double k) :etaI(etai), etaT(etat), k(k) {}
		virtual double evaluate(double cosTheta)const {

			return FConductor(cosTheta, etaI, etaT, k);
		}
	};

	class FresnelDielectric :public Fresnel {
	private:
		double etaI, etaT;
	public:
		FresnelDielectric(double etaI, double etaT) :etaI(etaI), etaT(etaT) {}
		virtual double evaluate(double cosTheta)const {
			return FDielectric(cosTheta, etaI, etaT);
		}

	};

	/// <summary>
	/// 理想模型，对于所有入射方向都完美反射，多用于镜面材质
	/// </summary>
	class FresnelNoOp final :public Fresnel {
	public:
		FresnelNoOp() {}
		double evaluate(double cosTheta)const { return 1.0; }
	};

	enum BxDFType {
		Reflection = 1 << 0,
		Transmission = 1 << 1,
		Diffuse = 1 << 2,
		Glossy = 1 << 3,
		Specular = 1 << 4,
		All = 1 << 5
	};

	class BxDF {
	public:
		BxDFType type;

		BxDF() {}
		BxDF(BxDFType type) :type(type) {}
		//compute brdf value
		virtual Spectrum f(const Vector3f& wo, const Vector3f& wi)const = 0;
		//sample wi,compute pdf and brdf values
		virtual Spectrum sampled_f(const Vector3f& wo, Vector3f& wi, const Point2f& sample, double& pdf)const = 0;
		virtual Spectrum hdf(const Vector3f& wo, int nSamples, Point2f* samples) { return Spectrum(0.0); }
		virtual Spectrum hhf(int nSamples, Point2f* outSamples, Point2f* inSamples) { return Spectrum(0.0); }

		virtual double pdf(const Vector3f& wo, const Vector3f& wi)const = 0;
		bool matchType(BxDFType t) { return t | type; }
	};

	class scaledBxDF :public BxDF {
	public:
		scaledBxDF(BxDFType type, BxDF* bxdf, Spectrum scaler) :BxDF(type), bxdf(bxdf), scaler(scaler) {}
		Spectrum f(const Vector3f& wo, Vector3f& wi) {
			return scaler * bxdf->f(wo, wi);
		}
	private:
		BxDF* bxdf;
		Spectrum scaler;
	};

	inline double CosTheta(const Vector3f& w) {
		return w.z;
	}
	inline double Cos2Theta(const Vector3f& w) {
		return w.z * w.z;
	}
	inline double AbsCosTheta(const Vector3f& w) {
		return abs(w.z);
	}
	inline double Sin2Theta(const Vector3f& w) {
		return Max(0.0, 1. - Cos2Theta(w));
	}
	inline double SinTheta(const Vector3f& w) {
		return sqrt(Sin2Theta(w));
	}
	inline double TanTheta(const Vector3f& w) {
		return SinTheta(w) / CosTheta(w);
	}
	inline double AbsTanTheta(const Vector3f& w) {
		return abs(SinTheta(w) / CosTheta(w));
	}
	inline double Tan2Theta(const Vector3f& w) {
		return TanTheta(w) * TanTheta(w);
	}
	inline double CosPhi(const Vector3f& w) {
		double sinTheta = SinTheta(w);
		return sinTheta == 0 ? 1 : Clamp(w.x / sinTheta, -1.f, 1.f);
	}
	inline double SinPhi(const Vector3f& w) {
		double sin = SinTheta(w);
		return sin == 0 ? 0 : Clamp(w.y / sin, -1.f, 1.f);
	}
	inline double Cos2Phi(const Vector3f& w) {
		return CosPhi(w) * CosPhi(w);
	}
	inline double Sin2Phi(const Vector3f& w) {
		return SinPhi(w) * SinPhi(w);
	}
}

#endif