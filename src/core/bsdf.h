#ifndef _RAVEN_CORE_BSDF_H_
#define _RAVEN_CORE_BSDF_H_

#include<vector>
#include<memory>
#include"base.h"
#include"transform.h"
#include"bxdf.h"
#include"math.h"

namespace Raven {
	class BSDF {
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			BSDF(const SurfaceInteraction& sits, double eta, int maxN = 1)
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
			genTBN(Vector3f(n), &sx, &sy);
			ns = n;
		}

		void addBxDF(std::shared_ptr<BxDF> bxdf) {
			bxdfs.push_back(bxdf);
			bxdfNumber++;
		}

		//Spectrum f(const vectorf3& wo, const vectorf3& wi) {
		//	vectorf3 wout = worldToLocal(-wo);//将入射光变换到BSDF坐标系下并使入射光朝向平面外侧
		//	Spectrum result = Spectrum(0.0);
		//	for (int i = 0; i < bxdfs.size(); i++) {
		//		result += bxdfs[i]->f(wo, wi);
		//	}
		//
		//	return result /= (double)bxdfs.size();
		//}

		Vector3f f(const Vector3f& wo, const Vector3f& wi) {
			Vector3f wout = worldToLocal(-wo);//将入射光变换到BSDF坐标系下并使入射光朝向平面外侧
			Vector3f result = Vector3f(0.0);
			for (int i = 0; i < bxdfs.size(); i++) {
				result += bxdfs[i]->f(wo, wi);
			}

			return result /= (double)bxdfs.size();
		}

		//Spectrum sample_f(const vectorf3& wo, vectorf3& wi, const pointf2& sample, double* pdf, BxDFType type = All)const {
		//	//choose a bxdf to sample
		//	int bxdfIndex = Min((unsigned int)(bxdfs.size() - 1), (unsigned int)std::floor(bxdfs.size() * sample[0]));
		//
		//	//sample choosen BxDF
		//	vectorf3 woLocal = worldToLocal(-wo);
		//	vectorf3 wiLocal;
		//	if (woLocal.z < 0)
		//		return Spectrum(0.);
		//	*pdf = 0.0;
		//	Spectrum f = bxdfs[bxdfIndex]->sampled_f(woLocal, wiLocal, sample, pdf);
		//	//if (*pdf == 0)
		//	//	return 0;
		//	wi = Normalize(localToWorld(wiLocal));
		//
		//	//compute overall pdf of sampled wi
		//	int bxdfNum = 1;
		//	for (int i = 0; i < bxdfs.size(); ++i) {
		//		if (i != bxdfIndex) {
		//			*pdf += bxdfs[i]->pdf(woLocal, wiLocal);
		//			bxdfNum++;
		//		}
		//	}
		//	*pdf /= (double)bxdfNum;
		//
		//	//compute value of sampled direction
		//	for (int i = 0; i < bxdfs.size(); ++i) {
		//		if (i != bxdfIndex) {
		//			f += bxdfs[i]->f(woLocal, wiLocal);
		//		}
		//	}
		//	f /= (double)bxdfNum;
		//	return f;
		//}

		Vector3f sample_f(const Vector3f& wo, Vector3f& wi, const Point2f& sample, double* pdf, BxDFType type = All)const {
			//choose a bxdf to sample
			int bxdfIndex = Min((unsigned int)(bxdfs.size() - 1), (unsigned int)std::floor(bxdfs.size() * sample[0]));

			//sample choosen BxDF
			Vector3f woLocal = worldToLocal(wo);
			Vector3f wiLocal;
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

	private:
		Vector3f sx, sy;	//axis formed the coordinate space
		Normal3f n, ns;		//surface normal and surface shading normal
		std::vector<std::shared_ptr<BxDF>> bxdfs;
		int bxdfNumber;
		double eta;
		const int maxNumber;

		Vector3f localToWorld(const Vector3f& v) const {
			return Vector3f(sx.x * v.x + sy.x * v.y + ns.x * v.z,
				sx.y * v.x + sy.y * v.y + ns.y * v.z,
				sx.z * v.x + sy.z * v.y + ns.z * v.z);
		}

		Vector3f worldToLocal(const Vector3f& v)const {
			return Vector3f(Dot(v, sx), Dot(v, sy), Dot(v, ns));
		}
	};
}

#endif