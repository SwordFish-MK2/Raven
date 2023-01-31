#include<Raven/medium/homogeneous.h>
#include<Raven/core/spectrum.h>

namespace Raven {
	HomogeneousMedium::HomogeneousMedium(
		const Spectrum& sa,
		const Spectrum& ss,
		double g) :
		sigma_a(sa),
		sigma_s(ss),
		sigma_t(sa + ss),
		g(g) {}

	Spectrum HomogeneousMedium::Tr(
		const Ray& ray,
		Sampler& sampler)const {
		double d = Min(ray.tMax * ray.dir.length(), std::numeric_limits<double>::max());
		return Exp(-sigma_t * d);
	}
}