#include<Raven/core/bsdf.h>

namespace Raven {
	BSDF::BSDF(const SurfaceInteraction& sits, double eta)
		:n(sits.n), ns(sits.n), eta(eta) {
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

	//������������䷽�򣬼���brdf
	Spectrum BSDF::f(const Vector3f& wo, const Vector3f& wi)const {
		Vector3f woLocal = worldToLocal(wo);//�������任��BSDF����ϵ�²�ʹ����⳯��ƽ�����
		Vector3f wiLocal = worldToLocal(wi);
		Spectrum result = Spectrum(0.0);
		bool reflect = Dot(wi, n) * Dot(wo, n) > 0;
		for (int i = 0; i < bxdfs.size(); i++) {
			if (reflect && bxdfs[i]->type & BxDFType::Reflection ||
				!reflect && bxdfs[i]->type & BxDFType::Transmission)
				result += bxdfs[i]->f(woLocal, wiLocal);
		}

		return result /= (double)bxdfs.size();
	}

	//�������䷽�򣬸���BRDF�ֲ��������䷽�򲢼���brdf��ֵ
	std::tuple<Spectrum, Vector3f, double, BxDFType> BSDF::sample_f(
		const Vector3f& wo,
		const Point2f& sample,
		BxDFType type)const {

		int nMatch = nMatchComponents(type);//n������������BxDF

		if (nMatch == 0) {
			//û�з���������Bxdf
			return std::tuple<Spectrum, Vector3f, double, BxDFType>(Spectrum(0.0), Vector3f(0.0), 0.0, BxDFType(0));
		}

		//�ӷ���������BxDF�в���һ��BxDF
		int compIndex = Min(nMatch - 1, (int)std::floor(nMatch * sample[0]));

		//��ȡ���ڲ�����BxDF��Index
		int sampleIndex = 0;
		for (size_t i = 0; i < bxdfs.size(); i++) {
			if (bxdfs[i]->matchType(type) && compIndex-- == 0) {
				sampleIndex = i;
				break;
			}
		}

		BxDFType sampledType = bxdfs[sampleIndex]->type;

		//����ѡ�е�BxDF
		Vector3f woLocal = Normalize(worldToLocal(wo));
		Vector3f wiLocal = Vector3f(0.0);

		double pdf = 0.0;
		Spectrum f = bxdfs[sampleIndex]->sampled_f(woLocal, wiLocal, sample, pdf);
		if (pdf == 0)
			return std::tuple<Spectrum, Vector3f, double, BxDFType>(Spectrum(0.0), Vector3f(0.0), pdf, BxDFType(0));

		Vector3f wi = Normalize(localToWorld(wiLocal));

		bool reflect = bxdfs[sampleIndex]->type & BxDFType::Reflection;

		//�����ϲ�����pdf
		for (int i = 0; i < bxdfs.size(); ++i) {
			if (i != sampleIndex) {
				if (reflect && bxdfs[i]->type & BxDFType::Reflection ||
					!reflect && bxdfs[i]->type & BxDFType::Transmission) {
					pdf += bxdfs[i]->pdf(woLocal, wiLocal);
				}
			}
		}
		pdf /= (int)nMatch;//����Monte Carlo���ֳ��Գ�ȡ����BxDF�ĸ���

		//���ݲ����ķ���������е�BxDF��ֵ
		for (int i = 0; i < bxdfs.size(); ++i) {
			if (i != sampleIndex) {
				if (reflect && bxdfs[i]->type & BxDFType::Reflection ||
					!reflect && bxdfs[i]->type & BxDFType::Transmission) {
					f += bxdfs[i]->f(woLocal, wiLocal);
				}
			}
		}

		return std::tuple<Spectrum, Vector3f, double, BxDFType>(f, wi, pdf, sampledType);
	}

	double BSDF::pdf(const Vector3f& wo, const Vector3f& wi)const {
		const Vector3f woLocal = Normalize(worldToLocal(wo));
		const Vector3f wiLocal = Normalize(worldToLocal(wi));
		if (woLocal.z == 0)
			return 0.0;
		double pdf = 0.0;
		for (int i = 0; i < bxdfNumber; i++) {
			pdf += bxdfs[i]->pdf(woLocal, wiLocal);
		}
		return bxdfNumber == 0 ? 0.0 : pdf / bxdfNumber;
	}

	int BSDF::nMatchComponents(BxDFType type)const {
		int num = 0;
		for (std::shared_ptr<BxDF> bxdf : bxdfs) {
			if (bxdf->matchType(type))
				num++;
		}
		return num;
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