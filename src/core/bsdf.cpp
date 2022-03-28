#include"bsdf.h"

namespace Raven {
	BSDF::BSDF(const SurfaceInteraction& sits, double eta, int maxN)
		:eta(eta), maxNumber(maxN), n(sits.n), ns(sits.n) {
		//generate coordinate space
		double maxLength = 0;
		int maxIndex = 0;
		bxdfNumber = 0;
		for (int i = 0; i < 3; i++)
			if (maxLength < n[i]) {
				maxLength = n[i];
				maxIndex = i;
			}
		n.normalize();
		auto [x, y] = genTBN(Vector3f(n));
		sx = x;
		sy = y;
		ns = n;
	}

	void BSDF::addBxDF(std::shared_ptr<BxDF> bxdf) {
		bxdfs.push_back(bxdf);
		bxdfNumber++;
	}

	//给定入射与出射方向，计算brdf
	Vector3f BSDF::f(const Vector3f& wo, const Vector3f& wi)const {
		Vector3f wout = worldToLocal(wo);//将入射光变换到BSDF坐标系下并使入射光朝向平面外侧
		Vector3f result = Vector3f(0.0);
		for (int i = 0; i < bxdfs.size(); i++) {
			result += bxdfs[i]->f(wo, wi);
		}

		return result /= (double)bxdfs.size();
	}

	//给定入射方向，根据BRDF分布采样出射方向并计算brdf的值
	Vector3f BSDF::sample_f(const Vector3f& wo, Vector3f& wi, const Point2f& sample,
		double* pdf, BxDFType type)const {
		//choose a bxdf to sample
		int bxdfIndex = Min((unsigned int)(bxdfs.size() - 1), (unsigned int)std::floor(bxdfs.size() * sample[0]));

		//sample choosen BxDF
		Vector3f woLocal = worldToLocal(wo);
		Vector3f wiLocal;
		//TODO:检查是否生成wi
		if (woLocal.z < 0)
			return Vector3f(0.0);
		*pdf = 0.0;
		Vector3f f = bxdfs[bxdfIndex]->sampled_f(woLocal, wiLocal, sample, pdf);
		//if (*pdf == 0)
		//	return 0;
		wi = Normalize(localToWorld(wiLocal));

		//compute overall pdf of sampled wi
		int bxdfNum = 1;
		for (int i = 0; i < bxdfs.size(); ++i) {
			if (i != bxdfIndex) {
				*pdf += bxdfs[i]->pdf(woLocal, wiLocal);
				bxdfNum++;
			}
		}
		*pdf /= (double)bxdfNum;

		//compute value of sampled direction
		for (int i = 0; i < bxdfs.size(); ++i) {
			if (i != bxdfIndex) {
				f += bxdfs[i]->f(woLocal, wiLocal);
			}
		}
		f /= (double)bxdfNum;
		return f;
	}

	double BSDF::pdf(const Vector3f& wo, const Vector3f& wi)const {
		const Vector3f woLocal = worldToLocal(wo);
		const Vector3f wiLocal = worldToLocal(wi);
		if (woLocal.z == 0)
			return 0.0;
		double pdf = 0.0;
		for (int i = 0; i < bxdfNumber; i++) {
			pdf += bxdfs[i]->pdf(woLocal, wiLocal);
		}
		return bxdfNumber == 0 ? 0.0 : pdf / bxdfNumber;
	}


	Vector3f BSDF::localToWorld(const Vector3f& v) const {
		return Vector3f(sx.x * v.x + sy.x * v.y + ns.x * v.z,
			sx.y * v.x + sy.y * v.y + ns.y * v.z,
			sx.z * v.x + sy.z * v.y + ns.z * v.z);
	}

	Vector3f BSDF::worldToLocal(const Vector3f& v)const {
		return Vector3f(Dot(v, sx), Dot(v, sy), Dot(v, ns));
	}
}