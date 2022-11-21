#ifndef _RAVEN_RENDERER_PATH_TRACING_H_
#define _RAVEN_RENDERER_PATH_TRACING_H_

#include<Raven/core/renderer.h>
#include<Raven/core/base.h>
#include<Raven/core/spectrum.h>
#include<Raven/utils/propertylist.h>

namespace Raven {
	class PathTracingRenderer : public Renderer {
	public:
		PathTracingRenderer(
			const std::shared_ptr<Camera>& c, 
			const std::shared_ptr<Film>& f, 
			int spp = 100, 
			int maxDepth = 64,
			double epslion = 1e-6) :
			Renderer(c, f, spp, epsilon), maxDepth(maxDepth) {}
		virtual void render(const Scene& scene);

	private:
		Spectrum integrate(const Scene& scene, const RayDifferential& r_in, int depth = 0)const;
		//	GeometryData gBuffer(const Ray& ray, const Scene& scene)const;

		const int maxDepth;
	};

	std::shared_ptr<Renderer> makePathTracingRenderer(const std::shared_ptr<Film>& film,
		const std::shared_ptr<Camera>& camera, const PropertyList& param);
}

#endif