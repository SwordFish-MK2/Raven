#ifndef _RAVEN_MEDIUM_HOMOGENEOUS_H_
#define _RAVEN_MEDIUM_HOMOGENEOUS_H_

#include<Raven/core/medium.h>
#include<Raven/core/base.h>
#include<Raven/core/ray.h>
namespace Raven {
	class HomogeneousMedium :public Medium {
	public:
		HomogeneousMedium(const Spectrum& sa, const Spectrum& ss, double g);

		virtual Spectrum Tr(const Ray& ray, Sampler& sampler)const override;
	private:
		const Spectrum sigma_a;//吸收
		const Spectrum sigma_s;//向外散射
		const Spectrum sigma_t;//衰减
		const double g;
	};
}

#endif