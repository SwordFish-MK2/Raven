#ifndef _RAVEN_CORE_BSDF_H_
#define _RAVEN_CORE_BSDF_H_

#include<vector>
#include<memory>
#include"base.h"
#include"transform.h"
#include"bxdf.h"
#include"math.h"
#include"spectrum.h"

namespace Raven {

	class BSDF {
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	public:

		BSDF(const SurfaceInteraction& sits, double eta, int maxN = 1);

		void addBxDF(std::shared_ptr<BxDF> bxdf);

		Spectrum f(const Vector3f& wo, const Vector3f& wi)const;

		Spectrum sample_f(const Vector3f& wo, Vector3f& wi, const Point2f& sample, double* pdf, BxDFType type = All)const;

		double pdf(const Vector3f& wo, const Vector3f& wi)const;
	private:
		Vector3f sx, sy;	//axis formed the coordinate space
		Normal3f n, ns;		//surface normal and surface shading normal
		std::vector<std::shared_ptr<BxDF>> bxdfs;
		int bxdfNumber;
		double eta;
		const int maxNumber;

		Vector3f localToWorld(const Vector3f& v) const;

		Vector3f worldToLocal(const Vector3f& v)const;
	};

}

#endif