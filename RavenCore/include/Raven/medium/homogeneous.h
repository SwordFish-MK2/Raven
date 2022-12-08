#ifndef _RAVEN_MEDIUM_HOMOGENEOUS_H_
#define _RAVEN_MEDIUM_HOMOGENEOUS_H_

#include<Raven/core/medium.h>

namespace Raven {
	class HomogeneousMedium :public Medium {
	public:
		HomogeneousMedium(const Spectrum& sa, const Spectrum& ss, double g);

		virtual Spectrum Tr(const Ray& ray, Sampler& sampler)const override;
	private:
		const Spectrum sigma_a;//Œ¸ ’
		const Spectrum sigma_s;//œÚÕ‚…¢…‰
		const Spectrum sigma_t;//À•ºı
		const double g;
	};
}

#endif