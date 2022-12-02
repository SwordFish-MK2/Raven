#ifndef _RAVEN_RENDERER_PATH_TRACING_H_
#define _RAVEN_RENDERER_PATH_TRACING_H_

#include<Raven/core/integrator.h>
#include<Raven/core/base.h>
#include<Raven/core/spectrum.h>
#include<Raven/utils/propertylist.h>

namespace Raven {
	class PathTracingIntegrator : public Integrator {
	public:
		PathTracingIntegrator(
			//const std::shared_ptr<Camera>& c, 
			//const std::shared_ptr<Film>& f, 
			int spp = 100,
			int maxDepth = 64,
			double epslion = 1e-6) :
			Integrator(spp, epsilon), maxDepth(maxDepth) {}
		void render(const Scene&, const Ref<Camera>&, Ref<Film>&)const override;

		static Ref<PathTracingIntegrator> construct(const PropertyList& param);

	private:
		Spectrum integrate(const Scene& scene, const RayDifferential& r_in, int depth = 0)const;
		//	GeometryData gBuffer(const Ray& ray, const Scene& scene)const;

		const int maxDepth;
	};

	_RAVEN_INTEGRATOR_REG_(path,PathTracingIntegrator,PathTracingIntegrator::construct)
}

#endif