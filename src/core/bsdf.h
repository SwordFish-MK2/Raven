#ifndef _RAVEN_CORE_BSDF_H_
#define _RAVEN_CORE_BSDF_H_

#include<vector>
#include<memory>
#include"base.h"
#include"transform.h"
#include"bxdf.h"
#include"math.h"
#include"spectrum.h"
#include<tuple>

namespace Raven {

	class BSDF {
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	public:

		BSDF(const SurfaceInteraction& sits, double eta = 1);

		void addBxDF(std::shared_ptr<BxDF> bxdf);

		Spectrum f(const Vector3f& wo, const Vector3f& wi)const;

		std::tuple<Spectrum, Vector3f, double, BxDFType> sample_f(const Vector3f& wo,
			const Point2f& sample, BxDFType type = All)const;

		double pdf(const Vector3f& wo, const Vector3f& wi)const;
		double getEta()const {
			return eta;
		}
	private:
		Vector3f sx, sy;	//axis formed the coordinate space
		Normal3f n, ns;		//surface normal and surface shading normal
		std::vector<std::shared_ptr<BxDF>> bxdfs;
		int bxdfNumber;
		double eta;
		int nMatchComponents(BxDFType type)const;

		Vector3f localToWorld(const Vector3f& v) const;

		Vector3f worldToLocal(const Vector3f& v)const;
	};

}

#endif
